#include <cstdio>
#include <string>

#include <catch.hpp>

#include "config/path.h"
#include "core/posixShell.h"
#include "core/task.h"
#include "utils/tmpFile.h"
#include "utils/utils.h"

using std::string;

using boost::filesystem::current_path;

using execHelper::config::Path;

using execHelper::test::utils::TmpFile;

namespace execHelper { namespace core { namespace test {
    SCENARIO("Test the posix shell for successfull commands", "[shell][posixshell]") {
        GIVEN("A posix shell and a file that can be shown in it") {
            const TmpFile file;

            PosixShell shell;

            WHEN("We apply it on a task to show the file") {
                Task task({"touch", file.getFilename()});
                task.setWorkingDirectory(file.getPath().parent_path());

                PosixShell::ShellReturnCode returnCode = shell.execute(task);

                THEN("The call should succeed") {
                    REQUIRE(shell.isExecutedSuccessfully(returnCode));
                }

                THEN("The file should exist") {
                    REQUIRE(file.exists());
                }
            }
        }
    }

    SCENARIO("Test the posix shell for shell expansion", "[shell][posixshell]") {
        GIVEN("A posix shell and a file that can be shown in it") {
            const TmpFile file(current_path());

            PosixShell shell;

            WHEN("We apply it on a task to show the file") {
                Task task({"touch", "$(pwd)/" + file.getFilename()});
                PosixShell::ShellReturnCode returnCode = shell.execute(task);

                THEN("The call should succeed") {
                    REQUIRE(shell.isExecutedSuccessfully(returnCode));
                }

                THEN("The file should exist") {
                    REQUIRE(file.exists());
                }
            }
        }
    }

    SCENARIO("Test the posix shell for word expansion", "[shell][posixshell]") {
        GIVEN("A posix shell and a file that can be shown in it") {
            TmpFile file(current_path());
            EnvironmentCollection env = {{"PWD", "."}};
            PosixShell shell;

            WHEN("We use a command with variable expansion") {
                Task task({"touch", "$PWD/" + file.getFilename()});
                task.setEnvironment(env);
                PosixShell::ShellReturnCode returnCode = shell.execute(task);

                THEN("The call should succeed") {
                    REQUIRE(shell.isExecutedSuccessfully(returnCode));
                }

                THEN("The file should exist") {
                    REQUIRE(file.exists());
                }
            }

            WHEN("We use a command with variable expansion") {
                Task task({"touch", "${PWD}/" + file.getFilename()});
                task.setEnvironment(env);

                PosixShell::ShellReturnCode returnCode = shell.execute(task);

                THEN("The call should succeed") {
                    REQUIRE(shell.isExecutedSuccessfully(returnCode));
                }

                THEN("The file should exist") {
                    REQUIRE(file.exists());
                }
            }

            WHEN("We use a command with command expansion") {
                Task task({"touch", "$(pwd)/" + file.getFilename()});

                PosixShell::ShellReturnCode returnCode = shell.execute(task);

                THEN("The call should succeed") {
                    REQUIRE(shell.isExecutedSuccessfully(returnCode));
                }

                THEN("The file should exist") {
                    REQUIRE(file.exists());
                }
            }

            WHEN("We use a command with variable not inherited from the outside environment") {
                env.emplace(make_pair("SELF_DEFINED_FILENAME", file.getFilename()));

                Task task({"touch", "./${SELF_DEFINED_FILENAME}"});
                task.setEnvironment(env);

                PosixShell::ShellReturnCode returnCode = shell.execute(task);

                THEN("The call should succeed") {
                    REQUIRE(shell.isExecutedSuccessfully(returnCode));
                }

                THEN("The file should exist") {
                    REQUIRE(file.exists());
                }
            }
        }
    }

    SCENARIO("Test the posix shell for unsuccessfull commands", "[shell][posixshell]") {
        GIVEN("A posix shell and a file that can be shown in it") {
            TmpFile file;
            PosixShell shell;

            WHEN("We apply it on a task to show the file") {
                Task task({"cat", file.toString()});

                PosixShell::ShellReturnCode returnCode = shell.execute(task);

                THEN("The call should not succeed") {
                    REQUIRE(returnCode != 0);
                    REQUIRE_FALSE(shell.isExecutedSuccessfully(returnCode));
                }
            }
        }
    }
} // namespace test
} // namespace core
} // namespace execHelper
