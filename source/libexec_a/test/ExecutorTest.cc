#include "gtest/gtest.h"

#include "../Resolver.h"
#include "../Session.h"
#include "../Environment.h"
#include "../Executor.h"

namespace {

    constexpr char LS_PATH[] = "/usr/bin/ls";
    constexpr char LS_FILE[] = "ls";
    constexpr char* LS_ARGV[] = {
            const_cast<char *>("/usr/bin/ls"),
            const_cast<char *>("-l"),
            nullptr
    };
    constexpr char* LS_ENVP[] = {
            const_cast<char *>("PATH=/usr/bin:/usr/sbin"),
            nullptr
    };
    constexpr char SEARCH_PATH[] = "/usr/bin:/usr/sbin";

    constexpr int FAILURE = -1;
    constexpr int SUCCESS = 0;

    struct BrokenResolver : public ear::Resolver {

        execve_t execve() const noexcept override {
            return nullptr;
        }

        posix_spawn_t posix_spawn() const noexcept override {
            return nullptr;
        }
    };

    class ExecutorTest : public ::testing::Test {
    public:
        static const ear::Session BROKEN_SESSION;
        static const ear::Session SILENT_SESSION;
        static const ear::Session VERBOSE_SESSION;
        static const BrokenResolver BROKEN_RESOLVER;
    };
    const ear::Session ExecutorTest::BROKEN_SESSION = { };
    const ear::Session ExecutorTest::SILENT_SESSION = {
            "/usr/libexec/libexec.so",
            "/usr/bin/intercept",
            "/tmp/intercept.random",
            false
    };
    const ear::Session ExecutorTest::VERBOSE_SESSION = {
            "/usr/libexec/libexec.so",
            "/usr/bin/intercept",
            "/tmp/intercept.random",
            true
    };
    const BrokenResolver ExecutorTest::BROKEN_RESOLVER = BrokenResolver();

    TEST_F(ExecutorTest, execve_fails_without_env) {
        auto result = ear::Executor(BROKEN_SESSION, BROKEN_RESOLVER).execve(LS_PATH, LS_ARGV, LS_ENVP);
        EXPECT_EQ(FAILURE, result);
    }

    TEST_F(ExecutorTest, execve_fails_without_resolver) {
        auto result = ear::Executor(SILENT_SESSION, BROKEN_RESOLVER).execve(LS_PATH, LS_ARGV, LS_ENVP);
        EXPECT_EQ(FAILURE, result);
    }

    TEST_F(ExecutorTest, execve_silent_library) {
        struct MockResolver : public ear::Resolver {
            ear::Resolver::execve_t execve() const noexcept override {
                return [](const char* path, char* const argv[], char* const envp[]) -> int {
                    EXPECT_STREQ(SILENT_SESSION.get_reporter(), path);
                    EXPECT_STREQ(SILENT_SESSION.get_reporter(), argv[0]);
                    EXPECT_STREQ(ear::FLAG_DESTINATION, argv[1]);
                    EXPECT_STREQ(SILENT_SESSION.get_destination(), argv[2]);
                    EXPECT_STREQ(ear::FLAG_LIBRARY, argv[3]);
                    EXPECT_STREQ(SILENT_SESSION.get_library(), argv[4]);
                    EXPECT_STREQ(ear::FLAG_PATH, argv[5]);
                    EXPECT_STREQ(LS_PATH, argv[6]);
                    EXPECT_STREQ(ear::FLAG_COMMAND, argv[7]);
                    EXPECT_STREQ(LS_ARGV[0], argv[8]);
                    EXPECT_STREQ(LS_ARGV[1], argv[9]);
                    EXPECT_EQ(LS_ENVP, envp);
                    return SUCCESS;
                };
            }

            ear::Resolver::posix_spawn_t posix_spawn() const noexcept override {
                return nullptr;
            }
        };
        const MockResolver mockResolver;

        auto result = ear::Executor(SILENT_SESSION, mockResolver).execve(LS_PATH, LS_ARGV, LS_ENVP);
        EXPECT_EQ(SUCCESS, result);
    }

    TEST_F(ExecutorTest, execve_verbose_library) {
        struct MockResolver : public ear::Resolver {
            ear::Resolver::execve_t execve() const noexcept override {
                return [](const char* path, char* const argv[], char* const envp[]) -> int {
                    EXPECT_STREQ(VERBOSE_SESSION.get_reporter(), path);
                    EXPECT_STREQ(VERBOSE_SESSION.get_reporter(), argv[0]);
                    EXPECT_STREQ(ear::FLAG_DESTINATION, argv[1]);
                    EXPECT_STREQ(VERBOSE_SESSION.get_destination(), argv[2]);
                    EXPECT_STREQ(ear::FLAG_LIBRARY, argv[3]);
                    EXPECT_STREQ(VERBOSE_SESSION.get_library(), argv[4]);
                    EXPECT_STREQ(ear::FLAG_VERBOSE, argv[5]);
                    EXPECT_STREQ(ear::FLAG_PATH, argv[6]);
                    EXPECT_STREQ(LS_PATH, argv[7]);
                    EXPECT_STREQ(ear::FLAG_COMMAND, argv[8]);
                    EXPECT_STREQ(LS_ARGV[0], argv[9]);
                    EXPECT_STREQ(LS_ARGV[1], argv[10]);
                    EXPECT_EQ(LS_ENVP, envp);
                    return SUCCESS;
                };
            }

            ear::Resolver::posix_spawn_t posix_spawn() const noexcept override {
                return nullptr;
            }
        };
        const MockResolver mockResolver;

        auto result = ear::Executor(VERBOSE_SESSION, mockResolver).execve(LS_PATH, LS_ARGV, LS_ENVP);
        EXPECT_EQ(SUCCESS, result);
    }

    TEST_F(ExecutorTest, execvpe_fails_without_env) {
        auto result = ear::Executor(BROKEN_SESSION, BROKEN_RESOLVER).execvpe(LS_FILE, LS_ARGV, LS_ENVP);
        EXPECT_EQ(FAILURE, result);
    }

    TEST_F(ExecutorTest, execvpe_fails_without_resolver) {
        auto result = ear::Executor(SILENT_SESSION, BROKEN_RESOLVER).execvpe(LS_FILE, LS_ARGV, LS_ENVP);
        EXPECT_EQ(FAILURE, result);
    }

    TEST_F(ExecutorTest, execvpe_passes) {
        struct MockResolver : public ear::Resolver {
            ear::Resolver::execve_t execve() const noexcept override {
                return [](const char* path, char* const argv[], char* const envp[]) -> int {
                    EXPECT_STREQ(SILENT_SESSION.get_reporter(), path);
                    EXPECT_STREQ(SILENT_SESSION.get_reporter(), argv[0]);
                    EXPECT_STREQ(ear::FLAG_DESTINATION, argv[1]);
                    EXPECT_STREQ(SILENT_SESSION.get_destination(), argv[2]);
                    EXPECT_STREQ(ear::FLAG_LIBRARY, argv[3]);
                    EXPECT_STREQ(SILENT_SESSION.get_library(), argv[4]);
                    EXPECT_STREQ(ear::FLAG_FILE, argv[5]);
                    EXPECT_STREQ(LS_FILE, argv[6]);
                    EXPECT_STREQ(ear::FLAG_COMMAND, argv[7]);
                    EXPECT_STREQ(LS_ARGV[0], argv[8]);
                    EXPECT_STREQ(LS_ARGV[1], argv[9]);
                    EXPECT_EQ(LS_ENVP, envp);
                    return SUCCESS;
                };
            }

            ear::Resolver::posix_spawn_t posix_spawn() const noexcept override {
                return nullptr;
            }
        };
        const MockResolver mockResolver;

        auto result = ear::Executor(SILENT_SESSION, mockResolver).execvpe(LS_FILE, LS_ARGV, LS_ENVP);
        EXPECT_EQ(SUCCESS, result);
    }

    TEST_F(ExecutorTest, execvp2_fails_without_env) {
        auto result = ear::Executor(BROKEN_SESSION, BROKEN_RESOLVER).execvP(LS_FILE, SEARCH_PATH, LS_ARGV, LS_ENVP);
        EXPECT_EQ(FAILURE, result);
    }

    TEST_F(ExecutorTest, execvp2_fails_without_resolver) {
        auto result = ear::Executor(SILENT_SESSION, BROKEN_RESOLVER).execvP(LS_FILE, SEARCH_PATH, LS_ARGV, LS_ENVP);
        EXPECT_EQ(FAILURE, result);
    }

    TEST_F(ExecutorTest, execvp2_passes) {
        struct MockResolver : public ear::Resolver {
            ear::Resolver::execve_t execve() const noexcept override {
                return [](const char* path, char* const argv[], char* const envp[]) -> int {
                    EXPECT_STREQ(SILENT_SESSION.get_reporter(), path);
                    EXPECT_STREQ(SILENT_SESSION.get_reporter(), argv[0]);
                    EXPECT_STREQ(ear::FLAG_DESTINATION, argv[1]);
                    EXPECT_STREQ(SILENT_SESSION.get_destination(), argv[2]);
                    EXPECT_STREQ(ear::FLAG_LIBRARY, argv[3]);
                    EXPECT_STREQ(SILENT_SESSION.get_library(), argv[4]);
                    EXPECT_STREQ(ear::FLAG_FILE, argv[5]);
                    EXPECT_STREQ(LS_FILE, argv[6]);
                    EXPECT_STREQ(ear::FLAG_SEARCH_PATH, argv[7]);
                    EXPECT_STREQ(SEARCH_PATH, argv[8]);
                    EXPECT_STREQ(ear::FLAG_COMMAND, argv[9]);
                    EXPECT_STREQ(LS_ARGV[0], argv[10]);
                    EXPECT_STREQ(LS_ARGV[1], argv[11]);
                    EXPECT_EQ(LS_ENVP, envp);
                    return SUCCESS;
                };
            }

            ear::Resolver::posix_spawn_t posix_spawn() const noexcept override {
                return nullptr;
            }
        };
        const MockResolver mockResolver;

        auto result = ear::Executor(SILENT_SESSION, mockResolver).execvP(LS_FILE, SEARCH_PATH, LS_ARGV, LS_ENVP);
        EXPECT_EQ(SUCCESS, result);
    }

    TEST_F(ExecutorTest, spawn_fails_without_env) {
        pid_t pid;
        auto result = ear::Executor(BROKEN_SESSION, BROKEN_RESOLVER).posix_spawn(
                &pid,
                LS_PATH,
                nullptr,
                nullptr,
                LS_ARGV,
                LS_ENVP);
        EXPECT_EQ(FAILURE, result);
    }

    TEST_F(ExecutorTest, spawn_fails_without_resolver) {
        pid_t pid;
        auto result = ear::Executor(SILENT_SESSION, BROKEN_RESOLVER).posix_spawn(
                &pid,
                LS_PATH,
                nullptr,
                nullptr,
                LS_ARGV,
                LS_ENVP);
        EXPECT_EQ(FAILURE, result);
    }

    TEST_F(ExecutorTest, spawn_passes) {
        struct MockResolver : public ear::Resolver {
            ear::Resolver::execve_t execve() const noexcept override {
                return nullptr;
            }

            ear::Resolver::posix_spawn_t posix_spawn() const noexcept override {
                return [](pid_t *pid,
                          const char *path,
                          const posix_spawn_file_actions_t *file_actions,
                          const posix_spawnattr_t *attrp,
                          char *const argv[],
                          char *const envp[]) -> int {
                    EXPECT_STREQ(SILENT_SESSION.get_reporter(), path);
                    EXPECT_STREQ(SILENT_SESSION.get_reporter(), argv[0]);
                    EXPECT_STREQ(ear::FLAG_DESTINATION, argv[1]);
                    EXPECT_STREQ(SILENT_SESSION.get_destination(), argv[2]);
                    EXPECT_STREQ(ear::FLAG_LIBRARY, argv[3]);
                    EXPECT_STREQ(SILENT_SESSION.get_library(), argv[4]);
                    EXPECT_STREQ(ear::FLAG_PATH, argv[5]);
                    EXPECT_STREQ(LS_PATH, argv[6]);
                    EXPECT_STREQ(ear::FLAG_COMMAND, argv[7]);
                    EXPECT_STREQ(LS_ARGV[0], argv[8]);
                    EXPECT_STREQ(LS_ARGV[1], argv[9]);
                    EXPECT_EQ(LS_ENVP, envp);
                    return SUCCESS;
                };
            }
        };
        const MockResolver mockResolver;

        pid_t pid;
        auto result = ear::Executor(SILENT_SESSION, mockResolver).posix_spawn(
                &pid,
                LS_PATH,
                nullptr,
                nullptr,
                LS_ARGV,
                LS_ENVP);
        EXPECT_EQ(SUCCESS, result);
    }

    TEST_F(ExecutorTest, spawnp_fails_without_env) {
        pid_t pid;
        auto result = ear::Executor(BROKEN_SESSION, BROKEN_RESOLVER).posix_spawnp(
                &pid,
                LS_FILE,
                nullptr,
                nullptr,
                LS_ARGV,
                LS_ENVP);
        EXPECT_EQ(FAILURE, result);
    }

    TEST_F(ExecutorTest, spawnp_fails_without_resolver) {
        pid_t pid;
        auto result = ear::Executor(SILENT_SESSION, BROKEN_RESOLVER).posix_spawnp(
                &pid,
                LS_FILE,
                nullptr,
                nullptr,
                LS_ARGV,
                LS_ENVP);
        EXPECT_EQ(FAILURE, result);
    }

    TEST_F(ExecutorTest, spawnp_passes) {
        struct MockResolver : public ear::Resolver {
            ear::Resolver::execve_t execve() const noexcept override {
                return nullptr;
            }

            ear::Resolver::posix_spawn_t posix_spawn() const noexcept override {
                return [](pid_t *pid,
                          const char *path,
                          const posix_spawn_file_actions_t *file_actions,
                          const posix_spawnattr_t *attrp,
                          char *const argv[],
                          char *const envp[]) -> int {
                    EXPECT_STREQ(SILENT_SESSION.get_reporter(), path);
                    EXPECT_STREQ(SILENT_SESSION.get_reporter(), argv[0]);
                    EXPECT_STREQ(ear::FLAG_DESTINATION, argv[1]);
                    EXPECT_STREQ(SILENT_SESSION.get_destination(), argv[2]);
                    EXPECT_STREQ(ear::FLAG_LIBRARY, argv[3]);
                    EXPECT_STREQ(SILENT_SESSION.get_library(), argv[4]);
                    EXPECT_STREQ(ear::FLAG_FILE, argv[5]);
                    EXPECT_STREQ(LS_FILE, argv[6]);
                    EXPECT_STREQ(ear::FLAG_COMMAND, argv[7]);
                    EXPECT_STREQ(LS_ARGV[0], argv[8]);
                    EXPECT_STREQ(LS_ARGV[1], argv[9]);
                    EXPECT_EQ(LS_ENVP, envp);
                    return SUCCESS;
                };
            }
        };
        const MockResolver mockResolver;

        pid_t pid;
        auto result = ear::Executor(SILENT_SESSION, mockResolver).posix_spawnp(
                &pid,
                LS_FILE,
                nullptr,
                nullptr,
                LS_ARGV,
                LS_ENVP);
        EXPECT_EQ(SUCCESS, result);
    }

}