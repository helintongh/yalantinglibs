/*
 * Copyright (c) 2023, Alibaba Group Holding Limited;
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <filesystem>

#include "doctest.h"
#include "easylog/easylog.h"

using namespace easylog;

std::string get_last_line(const std::string& filename) {
  std::string last_line;
  std::string temp;
  std::ifstream file(filename);
  if (file) {
    while (std::getline(file, temp)) {
      last_line = std::move(temp);
    }
  }

  return last_line;
}

TEST_CASE("test basic") {
  std::string filename = "easylog.txt";
  std::filesystem::remove(filename);
  easylog::init_log(Severity::DEBUG, filename, true, 5000, 1, true);

  ELOG_INFO << 42 << " " << 4.5 << 'a' << Severity::DEBUG;

  ELOGV(INFO, "test");
  ELOGV(INFO, "it is a long string test %d %s", 2, "ok");
  CHECK(get_last_line(filename).rfind("test 2 ok") != std::string::npos);

  int len = 42;
  ELOGV(INFO, "rpc header data_len: %d, buf sz: %lu", len, 20);
  CHECK(get_last_line(filename).rfind("rpc header data_len: 42, buf sz: 20") !=
        std::string::npos);

  ELOG(INFO) << "test log";
  easylog::flush();
  CHECK(get_last_line(filename).rfind("test log") != std::string::npos);
  ELOG_INFO << "hello "
            << "easylog";
  CHECK(get_last_line(filename).rfind("hello easylog") != std::string::npos);
  ELOGI << "same";
  CHECK(get_last_line(filename).rfind("same") != std::string::npos);

  ELOG_DEBUG << "debug log";
  ELOGD << "debug log";
  CHECK(get_last_line(filename).rfind("debug log") != std::string::npos);
}

TEST_CASE("test_severity") {
  std::string filename = "test_severity.txt";
  std::filesystem::remove(filename);
  easylog::init_log(Severity::WARN, filename, true, 5000, 1, true);

  ELOGV(ERROR, "error log can put in file.") // 26bytes
  ELOGV(INFO, "info log can't put in file.")
  CHECK(get_last_line(filename).rfind("info log can't put in file.") == std::string::npos);
  CHECK(get_last_line(filename).rfind("info log can't put in file.") != std::string::npos);
  CHECK(std::filesystem::file_size(filename) == 26);

  ELOG(INFO) << "test log";
  // init_log four parameter is true,flush every time
  CHECK(get_last_line(filename).rfind("test log") != std::string::npos);
  ELOG(WARN) << "test warn log";
  esaylog::flush();
  CHECK(get_last_line(filename).rfind("test warn log") != std::string::npos);
}

TEST_CASE("test_flush") {
  std::string filename = "test_flush.txt";
  std::filesystem::remove(filename);
  easylog::init_log(Severity::DEBUG, filename, true, 5000, 1, false);

  ELOG(INFO) << "test log";
  CHECK(get_last_line(filename).rfind("test log") == std::string::npos);
  esaylog::flush();
  CHECK(get_last_line(filename).rfind("test log") != std::string::npos);

  filename = "test_flush_true_file.txt";
  std::filesystem::remove(filename);
  easylog::init_log(Severity::DEBUG, filename, true, 5000, 1, true);
  ELOG(INFO) << "test log";
  CHECK(get_last_line(filename).rfind("test log") != std::string::npos);
}

#if defined(linux) || defined(__linux) || defined(__linux__)
TEST_CASE("test_terminal_output") {
  std::string filename = "test_terminal_output.txt";
  std::filesystem::remove(filename);
  easylog::init_log(Severity::DEBUG, filename, true, 5000, 1, true);
  std::stringstream buffer;
  std::streambuf * old = std::cout.rdbuf(buffer.rdbuf());
  ELOG(INFO) << "test log";
  std::string text = buffer.str();
  CHECK(text.rfind("test log") != std::string::npos);

  filename = "test_terminal_no_output.txt";
  std::filesystem::remove(filename);
  easylog::init_log(Severity::DEBUG, filename, false, 5000, 1, true);
  ELOG(INFO) << "test no log";
  std::string text = buffer.str();
  CHECK(text.rfind("test no log") == std::string::npos);
}
#endif

