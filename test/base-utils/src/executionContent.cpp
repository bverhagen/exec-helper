#include "executionContent.h" 

#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "base-utils/yaml.h"

using std::cout;
using std::endl;
using std::string;
using std::terminate;
using std::vector;

using boost::filesystem::exists;
using boost::filesystem::ofstream;

using execHelper::test::baseUtils::YamlReader;

namespace execHelper {
    namespace test {
        namespace baseUtils {
            ExecutionContent::ExecutionContent() noexcept :
                m_file("exec-helper.execution.%%%%") 
            {
                ofstream outputStream;
                outputStream.open(m_file.getPath());
                outputStream << "executions:" << std::endl;
                outputStream << getIterationContent() << std::endl;
                outputStream.close();
            }

            ExecutionContent::ExecutionContent(ExecutionContent&& other) noexcept {
                swap(other);
            }

            ExecutionContent& ExecutionContent::operator=(ExecutionContent&& other) noexcept {
                swap(other);
                return *this;
            }

            void ExecutionContent::swap(ExecutionContent& other) noexcept {
                m_file = std::move(other.m_file);
                other.m_file.clear();
            }

            std::string ExecutionContent::getPath() noexcept {
                return m_file.getPath().native();
            }

            string ExecutionContent::getIterationContent() noexcept {
                return "  - x";
            }

            unsigned int ExecutionContent::getNumberOfExecutions() noexcept {
                if(! exists(m_file.getPath())) {
                    return 0U;
                }
                try {
                    YamlReader yaml = YAML::LoadFile(m_file.getPath().native());
                    return yaml["executions"].as<vector<string>>().size() - 1U;
                } catch(const YAML::ParserException& e) {
                    cout << "Parser issue: " << e.what() << endl;
                    terminate();
                } catch(const YAML::TypedBadConversion<string>& e) {
                    cout << "Bad conversion: " << e.what() << endl;
                    terminate();
                } catch(const YAML::InvalidNode& e) {
                    cout << "Invalid node: " << e.what() << endl;
                    terminate();
                }
                return 0U;
            }
        } // namespace utils
    } // namespace test
} // namespace execHelper