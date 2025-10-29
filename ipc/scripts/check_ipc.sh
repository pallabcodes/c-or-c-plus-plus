#!/usr/bin/env bash
set -euo pipefail

root_dir="$(cd "$(dirname "$0")/../.." && pwd)"
ipc_dir="$root_dir/ipc"

echo "checking ipc sources under $ipc_dir"

rc=0

check_unchecked_returns() {
  local pattern="$1"
  local name="$2"
  if grep -R --include='*.c' --include='*.cpp' -n "${pattern}" "$ipc_dir" | grep -v "if (" >/dev/null 2>&1; then
    echo "potential unchecked ${name} return values detected"
    rc=1
  fi
}

check_unchecked_returns "pipe\(" "pipe"
check_unchecked_returns "fork\(" "fork"
check_unchecked_returns "mmap\(" "mmap"
check_unchecked_returns "shm_open\(" "shm_open"
check_unchecked_returns "sem_open\(" "sem_open"
check_unchecked_returns "msgget\(" "msgget"

missing_cleanup=$(grep -R --include='*.c' --include='*.cpp' -n "shm_open\(|mmap\(|sem_open\(|msgget\(" "$ipc_dir" | wc -l | tr -d ' ')
cleanup_calls=$(grep -R --include='*.c' --include='*.cpp' -n "shm_unlink\(|munmap\(|sem_close\(|sem_unlink\(|msgctl\(.*IPC_RMID" "$ipc_dir" | wc -l | tr -d ' ')

if [[ "$cleanup_calls" -lt "$missing_cleanup" ]]; then
  echo "potential missing cleanup calls detected"
  rc=1
fi

exit $rc


