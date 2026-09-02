# GlueX particle-associated hit simulation workflow

This repository contains a GlueX simulation/reconstruction workflow that produces **event-level, particle-level detector-hit records** for machine-learning studies. It supports a variable number of retained candidates per event rather than assuming one particle per event.

The detector targets are:

- **FDC:** charged-track hit information.
- **DIRC:** charged-particle optical/PMT hit information.
- **FCAL:** matched calorimeter showers and block hits.

Production normally uses a Singularity/Apptainer container containing the GlueX stack. Slurm scheduling happens **outside** the container: each allocated task starts the container and runs the GlueX MCwrapper workflow inside it. A site with a compatible GlueX installation can run the same workflow without the supplied container once the plugins, environment, calibration access, and configurations are installed.

## Repository layout

| Location | Purpose |
| --- | --- |
| `slurm_scripts/` | Host-side Slurm array submissions. |
| `config_files/` | MCwrapper generator and JANA reconstruction configuration. |
| `plugins/FCALextract/` | FCAL matched-shower and hit extraction. |
| `plugins/TrackMLextract/` | FDC track/hit extraction. |
| `plugins/pid_dirc/` | DIRC output for single particle-gun studies. |
| `plugins/dirc_tree/` | DIRC output for reaction studies. |
| `AIReadyProcessing/` | ROOT-to-JSON macros and JSON-to-NPZ conversion. |

Note that some of these plugins (pid_dirc, dirc_tree) already exist within halld_recon. I have modified these slightly so replace with these versions and reinstall. Instructions follow.

## Pipeline overview

```text
Slurm array job on host
  -> Singularity container (GlueX runtime)
    -> MCwrapper: generator + GEANT4 + reconstruction
      -> JANA plugins write ROOT TTrees
        -> MakeDictionaries_Charged.C / MakeDictionaries_Neutral.C
          -> newline-delimited JSON, one event per line
            -> parse_json.py
              -> compressed NPZ shard
```

## Download and install the GlueX Singularity container

Download the GlueX container image:

```bash
singularity build gluex_almalinux_9.sif \
  docker://jeffersonlab/gluex_almalinux_9
```

Create a persistent directory for the GlueX installation:

```bash
mkdir -p gluex_top
```

Install the GlueX software stack into that directory. This is the Singularity equivalent of the Docker installation command:

```bash
singularity exec \
  --bind "$PWD/gluex_top:/gluex_top" \
  gluex_almalinux_9.sif \
  env NTHREADS=4 \
  gluex_install/gluex_install.sh -r
```

The `gluex_top` directory is bind-mounted into the container at `/gluex_top`, so the installation persists after the container exits.

Use the resulting image and installation directory in production scripts:

```bash
SIF=/path/to/gluex_almalinux_9.sif
GLUEX_TOP=/path/to/gluex_top
```

Note that this can take some time depending on the number of threads you have available.

## Running a campaign

### Slurm and container

`submit_charged_array.sh` and `submit_neutral_array.sh` map `SLURM_ARRAY_TASK_ID` to a run number using `runs_to_index_unique.txt`. They bind the GlueX work area, simulation directory, and SQLite calibration databases into the container, then invoke `run_charged_simulation.sh` or `run_neutral_simulation.sh` inside it.

The current scripts request one CPU and pass 100,000 events to each run wrapper.

Submit from the host scheduler environment:

```bash
sbatch slurm_scripts/submit_charged_array.sh
sbatch slurm_scripts/submit_neutral_array.sh
```

Do not use MCwrapper inside the container to submit the local Slurm jobs in this workflow. Slurm allocates the job first; the container is its runtime environment.

### Site-specific fields to fill in

- Container image: `________________________________________`
- GlueX work-area path: `____________________________________`
- Run-number list: `_________________________________________`
- Simulation/scratch output: `________________________________`
- CCDB/RCDB connection or SQLite paths: `_____________________`
- Local run-wrapper command/interface: `______________________`
- Local ROOT/GlueX setup command: `___________________________`

Make sure you pull local versions of the SQL databases. Otherwise you will blacklist your nodes IP trying to pull from JLab. This is also extremely slow.

## Simulation configuration

MCwrapper configurations select the generator, GEANT4 simulation, calibration context, output location, and JANA configuration.

- `particle_gun.config`: neutral particle gun; currently uses `particle_gun/photon_flat.cfg`.
- `particle_gun_charged.config`: charged particle gun; currently uses `particle_gun/pip_flat.cfg`.
- `gen_2k.config`: reaction study using `gen_2k.cfg`, including a \(\phi \to K^+K^-\) topology.
- `gen_2pi_amp.config`: two-pion reaction study.

The files under `config_files/particle_gun/` set particle species and kinematic ranges. Update these before a new particle-gun campaign.

The reconstruction configuration must include:

```text
REST:WRITE_DIRC_HITS 1
FDC:MATCH_TRUTH_HITS 1
```

The first enables DIRC hit output. The second enables the FDC truth associations used by `TrackMLextract`.

Typical plugin sets:

| Study | Plugins |
| --- | --- |
| Single charged particle gun | `pid_dirc,TrackMLextract,FCALextract` |
| Neutral particle gun | `FCALextract` plus required standard reconstruction plugins |
| Multi-particle reaction | `dirc_tree,TrackMLextract,FCALextract` plus reaction/monitoring plugins |

`jana_reco_single.conf` is the single-particle configuration. `jana_reco.conf` is reaction-oriented. `jana_reac.conf` contains optional reaction-filter settings. Ensure the selected `PLUGINS` line matches the intended study.

## Installing and building plugins

Install these plugin directories in the GlueX reconstruction analysis-plugin source area:

```text
plugins/FCALextract
plugins/TrackMLextract
plugins/pid_dirc       # single-particle DIRC workflow
plugins/dirc_tree      # reaction DIRC workflow
```

Copy the complete selected DIRC plugin directory because it includes its `DrcEvent` and `DrcHit` ROOT classes. Do not enable both DIRC plugins in the same production unless this has been explicitly validated.

Example outline; replace paths for the local GlueX release:

```bash
source /path/to/gluex_env.sh

cp -a /path/to/Genesis/plugins/FCALextract /path/to/halld_recon/src/plugins/Analysis/
cp -a /path/to/Genesis/plugins/TrackMLextract /path/to/halld_recon/src/plugins/Analysis/
cp -a /path/to/Genesis/plugins/pid_dirc /path/to/halld_recon/src/plugins/Analysis/
cp -a /path/to/Genesis/plugins/dirc_tree /path/to/halld_recon/src/plugins/Analysis/

cd /path/to/halld_recon/src/<PLUGIN>
scons install -u
```

## Plugin outputs and matching

All custom plugins write ROOT TTrees into the reconstructed ROOT output. Downstream association uses JANA event number plus simulation track identifier.

### FCALextract

Writes `fcal_track_tree`, one row per reconstructed FCAL shower. Each row includes:

- event/shower IDs and reconstructed shower position, energy, time, and shape quantities;
- matching state, matched MC track ID, truth PID, and matched momentum;
- FCAL block row/column, position, energy, and time arrays.

Its configured geometric, time, and energy criteria determine the FCAL truth match. Neutral processing retains rows with a valid matched track. Charged processing also requires consistency with the selected DIRC track.

### TrackMLextract

Writes `fdc_track_tree`, including:

- `EventID`, reconstructed `TrackID`, selected `PID`, and `MCTrackID`;
- FDC layer, wire, time, charge, and position arrays;
- per-pseudo-hit truth track ID (`fdc_truth_track`);
- dominant-track `TruthPurity`;
- tracking quantities and matched momentum where available.

With `FDC:MATCH_TRUTH_HITS=1`, the plugin uses the `DMCTrackHit` association on FDC pseudo-hits. The dominant associated truth track labels the candidate, and only the tracking mass hypothesis matching its truth PID is selected. This prevents writing repeated mass-hypothesis rows for one physical trajectory.

`TruthPurity < 1` indicates that a reconstructed FDC candidate contains pseudo-hits attributed to more than one simulated track. Treat it as an important quality field for multi-track studies.

### pid_dirc and dirc_tree

Both create a `dirc` tree containing `DrcEvent` objects and an `EventID` branch. Records contain DIRC PMT/pixel/channel/time arrays, bar data, PID, MC track ID, and truth momentum.

- `pid_dirc`: used for particle guns; truth momentum comes from the matching generated primary.
- `dirc_tree`: used for reactions; truth momentum comes from the matched DIRC truth-bar hit.

For two DIRC tracks, charged JSON processing requires separate optical boxes. The expected box is selected from track `Y` sign and DIRC hits are retained when `pmtID / 108 == expectedOpticalBox`. Standard available DIRC PMT output does not carry the charged-parent ID of each optical photon, so tracks sharing an optical box cannot currently be split unambiguously and are rejected.

## ROOT to JSON

Run the ROOT macros after reconstruction:

```bash
root -l
.x AIReadyProcessing/MakeDictionaries_Charged.C("/path/to/hd_root.root", "charged.json")
.x AIReadyProcessing/MakeDictionaries_Neutral.C("/path/to/hd_root.root", "neutral.json")
```

The macro support files `DrcEvent.*`, `DrcHit.*`, and `glxtools.C` in `AIReadyProcessing/` must be available to ROOT.
You can compile these via:

```bash
root -l
.x AIReadyProcessing/DrcHit.cc+ AIReadyProcessing/DrcEvent.cc+ AIReadyProcessing/MakeDictionaries_Charged.C("/path/to/hd_root.root", "charged.json")
```

### JSON schema

Output is newline-delimited JSON: one object per event and a variable-length `Tracks` list.

```json
{
  "EventID": 42,
  "Tracks": [
    {
      "MCTrackID": 2,
      "PID": 321,
      "px": 0.1,
      "py": -0.2,
      "pz": 4.0,
      "DIRC": [{ "...": "DIRC data" }],
      "FDC": [{ "...": "FDC data" }],
      "FCAL": [{ "...": "FCAL data" }]
    }
  ]
}
```

- A single retained candidate produces `len(Tracks) == 1`.
- A retained two-candidate event produces `len(Tracks) == 2`.
- Charged tracks contain `FDC` and `DIRC`, plus `FCAL` if matched.
- Neutral tracks contain `FCAL` only.

Charged processing intentionally requires an FDC match, so DIRC-only candidates are not written. It also rejects events with more than two labelled DIRC tracks, or two selected DIRC tracks in the same optical box.

## JSON to NPZ

Convert JSON to a compressed NPZ shard:

```bash
python AIReadyProcessing/parse_json.py \
  --input_file charged.json \
  --output_file charged.npz \
  --detectors FDC DIRC FCAL
```

The default detector list is `FCAL FDC DIRC`. Passing only `--detectors FCAL` intentionally retains only FCAL-containing tracks and discards valid charged FDC+DIRC records without FCAL.

The script:

1. applies `GLOBAL_PID_LIST`;
2. removes empty selected detector arrays;
3. removes events with no retained tracks;
4. writes `event_ids` and an object array `events`.

Load the output with `allow_pickle=True`:

```python
import numpy as np

data = np.load("charged.npz", allow_pickle=True)
event_id = data["event_ids"][0]
tracks = data["events"][0]

# tracks is a Python list with one or more particle dictionaries.
for track in tracks:
    print(track["MCTrackID"], track["PID"])
```

The current PID filter is:

```python
[321, -321, 211, -211, 11, 2212, 22]
```

Update it before processing samples with species not on this list; otherwise those candidates will be removed.

## Combine NPZ files into training shards

Use `combine_npz_shards.py` to merge individual AI-ready NPZ files into larger compressed shards. The script preserves the existing arrays:

```python
event_ids
events
```

It processes input files in sorted filename order. Since EventIDs may restart at `0` or `1` in each input file, it offsets every subsequent file so that all output EventIDs are unique.

For example, if one file contains EventIDs `0–999` and the next file again contains `0–999`, the second file is shifted to `1000–1999`.

Create approximately 0.5 GB shards:

```bash
python AIReadyProcessing/combine_npz_shards.py \
  --folder_to_compress /path/to/input_npz_files \
  --output_file /path/to/combined/combined_photons.npz \
  --max_shard_gb 0.5
```

This writes numbered output files:

```text
combined_photons_00000.npz
combined_photons_00001.npz
combined_photons_00002.npz
...
```

The script starts a new shard only between source NPZ files. If one individual input file is larger than the requested shard size, it is written as its own shard and may exceed the target.

A 0.5 GB compressed shard is expected to contain roughly 1.2 GB of uncompressed NPZ payload, and may occupy approximately 2–4 GB RAM after unpickling into Python event dictionaries. Use smaller shards or load fewer shards per DDP rank if memory is limited.

If output files with the requested prefix already exist, choose a new output prefix or explicitly overwrite them:

```bash
python AIReadyProcessing/combine_npz_shards.py \
  --folder_to_compress /path/to/input_npz_files \
  --output_file /path/to/combined/combined_photons.npz \
  --max_shard_gb 0.5 \
  --overwrite
```

## Validation checklist

1. Run a small particle-gun sample for the intended species.
2. Confirm the selected JANA configuration enables the required plugins and truth flags.
3. Inspect `dirc`, `fdc_track_tree`, and `fcal_track_tree` in the ROOT output.
4. For charged data, confirm matching `EventID`, `MCTrackID`, and PID across detector trees.
5. Run the appropriate dictionary macro and inspect its printed counters.
6. Confirm JSON line count matches the macro's JSON-event count.
7. Convert with `parse_json.py` and verify retained event/track counts.
8. For multi-track reactions, examine optical-box rejection and FDC truth-purity statistics.

## Current scope and limitations

- The workflow is primarily validated with single-track particle guns.
- It has also been tested with \(\phi \to K^+K^-\) and \(\rho \to \pi^+\pi^-\)-style topologies.
- Same-optical-box multi-track DIRC events are rejected because present output cannot assign each optical photon to its charged parent.
- FDC candidates can have mixed truth content; inspect `TruthPurity` in multi-track analyses.
- Further tracker-matching validation and refinement is needed for more complex topologies.

