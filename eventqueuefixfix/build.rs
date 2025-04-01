use extshared_build_helper::*;

fn main() {
	let mut build = smext_build();
	link_sm_detours(&mut build);
	smext_css(&mut build);
	compile_lib(build, "smext");
}
