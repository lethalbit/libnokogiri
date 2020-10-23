#!/bin/bash
AFL_DIR=$MESON_BUILD_ROOT/afl
AFL_SYNC_DIR=$AFL_DIR/.sync
AFL_TESTCASE_DIR=$AFL_DIR/testcases
TEST_DATA_DIR=$MESON_SOURCE_ROOT/tests/test_data

if [ ! -d $AFL_DIR  ]; then
		mkdir -p $AFL_DIR $AFL_SYNC_DIR $AFL_TESTCASE_DIR
fi
echo "Copying initial test case data for $2"

case $2 in
	pcapng)
		cp $TEST_DATA_DIR/pcapng/*.pcapng $AFL_TESTCASE_DIR/
		;;
	pcap)
		find $TEST_DATA_DIR/pcap/ -type f -name '*.pcap' -a ! -name '*.ns.pcap' -exec cp \{\} $AFL_TESTCASE_DIR/ \;
		;;
	pcap-ns)
		cp $TEST_DATA_DIR/pcap/*.ns.pcap $AFL_TESTCASE_DIR/
		;;
	pcapng-gz)
		cp $TEST_DATA_DIR/pcapng/*.pcapng $AFL_TESTCASE_DIR/
		;;
	pcap-gz)
		find $TEST_DATA_DIR/pcap/ -type f -name '*.pcap.gz' -a ! -name '*.ns.pcap.gz' -exec cp \{\} $AFL_TESTCASE_DIR/ \;
		;;
	pcap-ns-gz)
		cp $TEST_DATA_DIR/pcap/*.ns.pcap.gz $AFL_TESTCASE_DIR/
		;;
esac


CPU_COUNT=$(cat /proc/cpuinfo | awk '/^processor/{print $3}' | tail -1)
echo "Found $CPU_COUNT CPU Cores"

echo "Tweaking System, I'mma need root for that"
sudo -v

echo "Setting core dump pattern"
echo core | sudo tee /proc/sys/kernel/core_pattern

for i in $(seq 0 $CPU_COUNT); do
	echo "Setting scaling_governor to \"performance\" for CPU $i"
	TGT_CPU="cpu$i"
	echo performance | sudo tee /sys/devices/system/cpu/$TGT_CPU/cpufreq/scaling_governor
done

echo "Dropping root, peace~"
sudo -k

for i in $(seq 1 $(($CPU_COUNT-1))); do
	WORKER_NAME="fuzz-worker-$i"
	echo "Spawning AFL worker $WORKER_NAME"
	afl-fuzz -i $AFL_TESTCASE_DIR -o $AFL_SYNC_DIR -S $WORKER_NAME $1 @@ > /dev/null 2>&1 &
	WORKER_PID=$!
	echo "Setting $WORKER_NAME (PID: $WORKER_PID) affinity to CPU $i"
	taskset -cp $i $WORKER_PID
done

echo "Starting AFL Orchestrator"
afl-fuzz -i $AFL_TESTCASE_DIR -o $AFL_SYNC_DIR -M fuzz-orchestrator $1 @@

echo "Fuzzers now running...."
