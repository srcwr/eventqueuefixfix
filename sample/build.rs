use extshared_build_helper::*;

fn main() {
	let build = smext_build();
	compile_lib(build, "smext");
}
