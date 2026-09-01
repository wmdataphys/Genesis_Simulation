import json
import numpy as np
import argparse
#                  K+   K-   pi+  pi- e- p+ gamma
GLOBAL_PID_LIST = [321,-321,211,-211,11,2212,22]

def parse_json_file(file_path, keys_to_check=["FCAL", "FDC", "DIRC"]):
    filtered_data = []

    with open(file_path, "r") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue

            event = json.loads(line)
            retained_tracks = []

            for track in event.get("Tracks", []):
                if track.get("PID") not in GLOBAL_PID_LIST:
                    continue
                # Remove empty detector arrays.
                for key in keys_to_check:
                    if isinstance(track.get(key), list) and not track[key]:
                        del track[key]

                # Keep a track only if it has data in at least one requested detector.
                if any(key in track for key in keys_to_check):
                    retained_tracks.append(track)

            # Keep an event only if at least one track survived.
            if retained_tracks:
                event["Tracks"] = retained_tracks
                filtered_data.append(event)

    print(f"Retained {len(filtered_data)} non-empty events.")
    return filtered_data


def process_event_to_npz(filtered_events, npz_output_path, detectors_to_track=["FCAL", "FDC", "DIRC"]):
    """
    Takes a list of filtered event dictionaries and packs them into a generic NumPy .npz 
    shard file, preserving native dictionaries/lists using NumPy object arrays.
    """
    if not filtered_events:
        return

    event_ids = np.array([e.get("EventID", 0) for e in filtered_events], dtype=np.int32)
    tracks = [e.get("Tracks", []) for e in filtered_events]
    
    # Create an empty object array to hold native Python lists for each event
    events_payload = np.empty(len(filtered_events), dtype=object)

    for i, event in enumerate(filtered_events):
        events_payload[i] = event.get("Tracks", [])

    # Save using object arrays (NumPy pickles objects internally within the npz)
    assert len(event_ids) == len(events_payload), "Mismatch in lengths of event_ids and events_payload"
    assert len(tracks) == len(events_payload), "Mismatch in lengths of momentum vectors and events_payload"

    np.savez_compressed(
        npz_output_path,
        event_ids=event_ids,
        events=events_payload
    )
    print(f"Saved generic shard: {npz_output_path}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Parse a JSON file and remove empty detector lists.")
    parser.add_argument("--input_file", type=str, help="Path to the input JSON file.")
    parser.add_argument("--output_file", type=str, help="Path to the output JSON file.")
    parser.add_argument("--detectors", type=str, nargs="*", default=["FCAL", "FDC", "DIRC"], help="List of detectors to track (default: FCAL, FDC, DIRC).")
    args = parser.parse_args()

    print("Tracking detectors:", args.detectors)

    filtered_data = parse_json_file(args.input_file, args.detectors)
    process_event_to_npz(filtered_data, args.output_file, args.detectors)