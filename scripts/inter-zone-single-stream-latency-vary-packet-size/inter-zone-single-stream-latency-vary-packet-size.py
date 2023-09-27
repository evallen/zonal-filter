""" inter-zone-single-stream-latency-vary-packet-size.py

Script to run the examples/inter-zone-single-stream-latency.cc simulation
many times with varying packet sizes.

Make sure to run this from the top level |ns3| directory!
"""

import argparse
from pathlib import Path
import subprocess
from tqdm import tqdm

ZONAL_DIRECTORY = "contrib/zonal-research"
DEFAULT_OUTPUT_DIRECTORY = f"{ZONAL_DIRECTORY}/results/inter-zone-single-stream-latency/packet-size/secure"
SINGLE_TEST_SCRIPT_PATH = f"{ZONAL_DIRECTORY}/examples/inter-zone-single-stream-latency.cc"

PACKET_SIZES = [64, 128, 256, 512, 1024, 1280, 1518]


# === Methods ====================================================================

def run_trial(packet_size, args):
    # Ensure result directory exists
    result_dir = Path(f"{args.output_directory}/size-{packet_size}/")
    result_dir.mkdir(parents=True, exist_ok=True)

    subprocess.run(
        [
            "./ns3",
            "run",
            f"'{SINGLE_TEST_SCRIPT_PATH} --packet-size={packet_size}'",
            "--cwd",
            f"{result_dir}",
        ]
    )

def run_trials(args):
    for packet_size in tqdm(PACKET_SIZES):
        run_trial(packet_size, args)


# === Main =======================================================================

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog='inter-zone-single-stream-latency-vary-packet-size',
        description='Measures latency of a single inter-zonal UDP stream with various' +
                    ' packet sizes.')

    parser.add_argument('--output-directory',
                        type=str,
                        help='the output directory for the test results',
                        default=DEFAULT_OUTPUT_DIRECTORY)

    args = parser.parse_args()

    run_trials(args)
