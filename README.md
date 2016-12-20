# Utility Headers for C++

Maintained at <https://github.com/rpavlik/util-headers>

Even when using the STL and libraries of best practices (like
[Boost][1], [Eigen][2], and [Loki][3]), a C++ coder sometimes requires
utility functionality that can be relatively neatly developed and tested
in isolation. Rather than give each small header its own project, we
collected such utility functionality, along with associated tests, in
this project.

The headers in `util/` provide small nuggets of functionality. The
headers are effectively independent, for the most part: you can use one
or all of them, without worrying about dependencies.

(Some exceptions apply, such as `BlockingInvokeFunctor` and its
implementation(s), which depend on each other but which should be
self-explanatory.)

Most, particularly the older ones, are C++98, while newer ones may use
C++11 features.

## Namespaces

Everything that is intended to be "public" is in the `util` namespace.
Code in `util::detail` is considered to be "implementation details" and
not necessarily for direct use.

## Building

Since these are only headers, a build system isn't strictly necessary:
just include them in your project. The build system included in this
repository (uses CMake to generate makefiles or project files) does
five things:

- provides an install target that stamps git revisions on each header.
- provides a way to easily edit the headers in a project-oriented IDE
- provides a `doc` target that runs [Doxygen](http://www.doxygen.org)
- verifies that each header can cleanly build in a translation unit of its own
- builds unit tests (and allows running them easily with CTest)

The following dependencies are all optional, but their presence will
enable whatever testing exists in the build system for the associated
headers:

- [Boost][1]:

	- boost::test required to build unit tests

	- boost::function headers required by `util/BlockingInvokeFunctor*.h`

- [Eigen][2]: header-only library (snapshot included in repository), used by:

	- `util/EigenTie.h`

	- `util/Saturate.h`

- [GMTL][4]: header-only library (often used with [VR Juggler][5]) used by:

	- `gmtlToOsgMatrix.h`

- [OpenSceneGraph][6]: used by:

	- `gmtlToOsgMatrix.h`

	- `osgFindNamedNode.h`

- [VPR (part of VR Juggler)][5] used by:

	- `util/BlockingInvokeFunctorVPR.h`

## Search Tool

Include guards in this project take a very specific pattern that includes
a GUID/UUID, both for optimal collision avoidance and for the bonus ability
to recognize even renamed files. The `util_header_search_tool.sh` (requires
something unix-like, Git Bash will work on Windows) takes advantage of this.

The tool can be useful to find which headers might need to be updated from
this "upstream" repo.

- Run with `--update` (after adding a new header) to parse headers and
generate the `guid.list.txt` and `guid.transforms.txt` data files, used
by the normal/search operating mode of the script.
- Running with any other arguments will lead to those arguments being
treated as input files passed to a pipeline starting with `grep`, that
will output the name of any `util-headers` file found (and its original
name in this repo)


## Licenses

The headers in `util/` are free and open-source software.
They may be distributed under the Boost Software License, Version 1.0;
see individual files for copyright notices.

A few headers extend [Eigen][2], which is
bundled in the `third-party` directory for testing purposes. It has its
own license.

A copy of the [Loki library][3] is
bundled in the `third-party` directory. It has its own license.

Some headers extend or interact with other libraries. Each of these
libraries is of course governed by its own license.

## Acknowledgement
If you find this useful, we would appreciate hearing from you - a GitHub
issue will do, as the primary author (Ryan Pavlik) is no longer in academia
and thus no longer needs publication copies (though they'd certainly be
appreciated).

[1]: http://www.boost.org/ "Boost libraries"
[2]: http://eigen.tuxfamily.org/ "Eigen matrix and vector math template library"
[3]: http://loki-lib.sourceforge.net/ "Loki library"
[4]: http://ggt.sourceforge.net/ "GMTL matrix and vector templates"
[5]: http://vrjuggler.googlecode.com/ "VR Juggler"
[6]: http://www.openscenegraph.org/ "OpenSceneGraph"
