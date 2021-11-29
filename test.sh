#!/usr/bin/env bash
set -e
cd $(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
export PATH=$PATH:$(pwd)/bin 
TIMEOUT=700


BASH_TEST_PREFIX="command env command bash --noprofile"

if [[ "$1" == shell ]]; then
  rc=$(mktemp)
  cat << EOF > $rc
BUILTIN_MODULES="\$(find src/.libs/ -type f -name "*.so")"
echo -e "\$BUILTIN_MODULES"|while read -r m; do 
  name="\$(basename \$m .so)"
  cmd="{ enable -f \$m \$name && enable -d \$name; }; enable -f \$m \$name"
  echo "\$cmd"
  eval "\$cmd"
done
ansi --cyan --bg-black "\$BUILTIN_MODULES"
ansi --blue --bold "\$LOAD_CMDS"
EOF
  rc_dat="$(ansi --yellow --italic "$(cat $rc)")"
  echo -e "Starting bash with rc file contents:\n$rc_dat"
  cmd="$BASH_TEST_PREFIX --rcfile $rc -i"
  eval "$cmd"
  unlink $rc
  exit 0
fi

COLORS=0
DEFAULT_POST_CMD="echo -e \"MYPID=\$MYPID\nTS=\$TS\nMS=\$MS\""

ansi --cyan --bold "Epoch MS: $(date +%s%3N)"
ansi --magenta --bold "Epoch: $(date +%s)"


test_builtin() {
	local M="$1"
	local N="$2"
	local post_cmd="${3:-$DEFAULT_POST_CMD}"
	local cmd="enable -f 'src/.libs/$M.so' $N && $N && $post_cmd"
	cmd="command env command bash --norc --noprofile -c '$cmd'"

	err() {
		pfx="$(ansi --red --bold "$1")"
		msg="$(ansi --yellow --italic "$2")"
		if [[ "$COLORS" == 1 ]]; then
			echo -e "[$pfx]   $msg"
		else
			echo -e "[$1]   $2"
		fi
	}

	ok() {
		pfx="$(ansi --green --bold "$1")"
		msg="$(ansi --yellow --italic "$2")"
		if [[ "$COLORS" == 1 ]]; then
			echo -e "[$pfx]   $msg"
		else
			echo -e "[$1]   $2"
		fi
	}

	while read -r l; do
		ok "TEST" "$l"
	done < <(eval "$cmd")

}

do_test(){
  test_builtin passh passh "passh exec $@"

}

log_level_tests(){
  export LOG_LEVEL=TRACE && do_test "id"
  export LOG_LEVEL=DEBUG && do_test "id"
  export LOG_LEVEL=WARN && do_test "id"
  export LOG_LEVEL=ERROR && do_test "id"
  export LOG_LEVEL=FATAL && do_test "id"
}

main(){
  do_test "--help"
  do_test "-L /tmp/p2.log -P 'password:' -p env:__PASS -c 20 -C -i -y -T -t $TIMEOUT -- ./wrap.sh"
}

export __PASS=$(uuidgen)
export LOG_LEVEL=TRACE
main
