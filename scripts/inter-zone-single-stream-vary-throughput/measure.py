""" measure.py

Script to run the examples/inter-zone-single-stream.cc simulation
many times with varying throughputs.

Make sure to run this from the top level |ns3| directory!
"""

import argparse
from pathlib import Path
import subprocess
from tqdm import tqdm
import numpy as np

ZONAL_DIRECTORY = "contrib/zonal-research"
BASE_OUTPUT_DIRECTORY = f"{ZONAL_DIRECTORY}/results/inter-zone-single-stream/throughput"
SINGLE_TEST_SCRIPT_PATH = f"{ZONAL_DIRECTORY}/examples/inter-zone-single-stream.cc"

PACKET_SIZE = 512
THROUGHPUTS = list(np.arange(10e6, 110e6, 10e6)) + list(np.arange(120e6, 220e6, 20e6))


# === Methods ====================================================================

def run_trial_insecure(throughput, args):
    # Ensure result directory exists
    result_dir = Path(f"{args.output_directory}/insecure/throughput-{throughput}/")
    result_dir.mkdir(parents=True, exist_ok=True)

    optional_args = ""
    if args.verbose:
        optional_args += " --verbose"
    test_string = f"'{SINGLE_TEST_SCRIPT_PATH} --insecure --packet-size={PACKET_SIZE} --sending-rate={throughput} {optional_args}'"

    subprocess.run(
        [
            "./ns3",
            "run",
            test_string,
            "--cwd",
            f"{result_dir}",
        ]
    )

def run_trial_secure(throughput, args):
    # Ensure result directory exists
    result_dir = Path(f"{args.output_directory}/secure/throughput-{throughput}/")
    result_dir.mkdir(parents=True, exist_ok=True)

    optional_args = ""
    if args.verbose:
        optional_args += " --verbose"
    test_string = f"'{SINGLE_TEST_SCRIPT_PATH} --packet-size={PACKET_SIZE} --sending-rate={throughput} {optional_args}'"

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
    for throughput in tqdm(THROUGHPUTS):
        throughput_str = f"{int(throughput)}bps"
        print(f"Trying throughput {throughput_str}...")
        run_trial_secure(throughput_str, args)
        run_trial_insecure(throughput_str, args)


# === Main =======================================================================

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog='inter-zone-single-stream-vary-packet-size',
        description='Measures characteristics of a single inter-zonal UDP stream with various' +
                    ' throughputs.')

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
