""" measure.py

Script to run the examples/inter-zone-single-stream-latency.cc simulation
many times with varying packet sizes.

Make sure to run this from the top level |ns3| directory!
"""

import argparse
from pathlib import Path
import subprocess
from tqdm import tqdm

ZONAL_DIRECTORY = "contrib/zonal-research"
BASE_OUTPUT_DIRECTORY = f"{ZONAL_DIRECTORY}/results/inter-zone-single-stream-latency/packet-size"
SINGLE_TEST_SCRIPT_PATH = f"{ZONAL_DIRECTORY}/examples/inter-zone-single-stream-latency.cc"

PACKET_SIZES = range(50, 1472, 50)


# === Methods ====================================================================

def run_trial_insecure(packet_size, args):
    # Ensure result directory exists
    result_dir = Path(f"{args.output_directory}/insecure/size-{packet_size}/")
    result_dir.mkdir(parents=True, exist_ok=True)

    optional_args = ""
    if args.verbose:
        optional_args += " --verbose"
    test_string = f"'{SINGLE_TEST_SCRIPT_PATH} --insecure --packet-size={packet_size} {optional_args}'"

    subprocess.run(
        [
            "./ns3",
            "run",
            test_string,
            "--cwd",
            f"{result_dir}",
        ]
    )

def run_trial_secure(packet_size, args):
    # Ensure result directory exists
    result_dir = Path(f"{args.output_directory}/secure/size-{packet_size}/")
    result_dir.mkdir(parents=True, exist_ok=True)

    optional_args = ""
    if args.verbose:
        optional_args += " --verbose"
    test_string = f"'{SINGLE_TEST_SCRIPT_PATH} --packet-size={packet_size} {optional_args}'"

    subprocess.run(
        [
            "./ns3",
            "run",
            test_string,
            "--cwd",
            f"{result_dir}",
        ]
    )

def run_trials(args):
    for packet_size in tqdm(PACKET_SIZES):
        run_trial_secure(packet_size, args)
        run_trial_insecure(packet_size, args)


# === Main =======================================================================

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog='inter-zone-single-stream-latency-vary-packet-size',
        description='Measures latency of a single inter-zonal UDP stream with various' +
                    ' packet sizes.')

    parser.add_argument('--output-directory',
                        type=str,
                        help='the output directory for the test results',
                        default=BASE_OUTPUT_DIRECTORY)

    parser.add_argument('--verbose',
                        type=str,
                        help='if the output should be verbose',
                        default=False)

    args = parser.parse_args()

    run_trials(args)
