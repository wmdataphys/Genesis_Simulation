import argparse
from pathlib import Path

import numpy as np


GIB = 1024 ** 3


def make_object_array(items):
    """Keep one Python event object in each array element."""
    result = np.empty(len(items), dtype=object)
    result[:] = items
    return result


def write_shard(output_prefix, shard_number, event_ids, events):
    output_path = output_prefix.parent / f"{output_prefix.name}_{shard_number:05d}.npz"

    np.savez_compressed(
        output_path,
        event_ids=np.asarray(event_ids, dtype=np.int64),
        events=make_object_array(events),
    )

    print(
        f"Wrote {output_path.name}: {len(event_ids)} events, "
        f"{output_path.stat().st_size / GIB:.3f} GiB"
    )


def main():
    parser = argparse.ArgumentParser(
        description="Combine NPZ event files and make EventIDs unique."
    )
    parser.add_argument(
        "--folder_to_compress",
        required=True,
        type=Path,
        help="Folder containing the input .npz files.",
    )
    parser.add_argument(
        "--output_file",
        required=True,
        type=Path,
        help=(
            "Output filename prefix. For example, output/dataset.npz creates "
            "dataset_00000.npz, dataset_00001.npz, etc."
        ),
    )
    parser.add_argument(
        "--max_shard_gb",
        required=True,
        type=float,
        help="Approximate maximum compressed size of each output shard in GiB.",
    )
    parser.add_argument(
        "--overwrite",
        action="store_true",
        help="Allow overwriting previously created shards with this prefix.",
    )
    args = parser.parse_args()

    if args.max_shard_gb <= 0:
        parser.error("--max_shard_gb must be positive.")

    input_folder = args.folder_to_compress.resolve()
    if not input_folder.is_dir():
        parser.error(f"Input folder does not exist: {input_folder}")

    output_prefix = args.output_file.resolve()
    if output_prefix.suffix == ".npz":
        output_prefix = output_prefix.with_suffix("")
    output_prefix.parent.mkdir(parents=True, exist_ok=True)

    # Do not accidentally combine shards created by an earlier run.
    input_files = [
        path for path in sorted(input_folder.glob("*.npz"))
        if path.parent.resolve() != output_prefix.parent
        or not path.name.startswith(f"{output_prefix.name}_")
    ]
    if not input_files:
        parser.error(f"No .npz files found in {input_folder}")

    existing_shards = list(output_prefix.parent.glob(f"{output_prefix.name}_*.npz"))
    if existing_shards and not args.overwrite:
        parser.error(
            f"Output shards already exist for {output_prefix}. "
            "Choose a new --output_file or use --overwrite."
        )
    if args.overwrite:
        for shard in existing_shards:
            shard.unlink()

    max_shard_bytes = args.max_shard_gb * GIB
    shard_event_ids = []
    shard_events = []
    shard_input_bytes = 0
    shard_number = 0

    # This is the only cross-file EventID state we need.
    last_global_event_id = None

    for input_path in input_files:
        with np.load(input_path, allow_pickle=True) as data:
            if "event_ids" not in data or "events" not in data:
                raise ValueError(
                    f"{input_path} must contain both 'event_ids' and 'events'."
                )

            local_event_ids = np.asarray(data["event_ids"], dtype=np.int64)
            local_events = np.asarray(data["events"], dtype=object)

        if len(local_event_ids) != len(local_events):
            raise ValueError(
                f"{input_path}: event_ids and events have different lengths."
            )
        if len(local_event_ids) == 0:
            print(f"Skipping empty file: {input_path.name}")
            continue

        # Preserve the first file's IDs. For every later file, shift its
        # local IDs so its first event follows the previous file's last event.
        if last_global_event_id is None:
            event_id_offset = 0
        else:
            event_id_offset = last_global_event_id + 1 - local_event_ids[0]

        global_event_ids = local_event_ids + event_id_offset

        # Start a new shard only at a source-file boundary. The source file's
        # compressed size is a simple estimate of what it adds. A single large
        # input file is written by itself.
        source_size = input_path.stat().st_size
        if shard_events and shard_input_bytes + source_size > max_shard_bytes:
            write_shard(
                output_prefix, shard_number, shard_event_ids, shard_events
            )
            shard_number += 1
            shard_event_ids = []
            shard_events = []
            shard_input_bytes = 0

        shard_event_ids.extend(global_event_ids.tolist())
        shard_events.extend(local_events.tolist())
        shard_input_bytes += source_size
        last_global_event_id = int(global_event_ids[-1])

        print(
            f"Added {input_path.name}: {len(local_event_ids)} events, "
            f"EventID offset {event_id_offset:+d}, "
            f"last global EventID {last_global_event_id}"
        )

    if shard_events:
        write_shard(output_prefix, shard_number, shard_event_ids, shard_events)


if __name__ == "__main__":
    main()

