#include "Engine.h"

#include <map>

#include "Fwk/Exception.h"

#include "Activity.h"
#include "ActivityReactor.h"
#include "Test.h"
#include "TestFileReader.h"

namespace Check50 {

Engine::Engine(const std::string& name,
               std::string& source_code,
               Engine::EngineMode engine_mode,
               Engine::TestFileFormat test_file_format):
    _name(name),
    _source_code(source_code),
	_notifiee(NULL),
    _engine_mode(engine_mode),
    _test_file_format(test_file_format),
    _activity_manager(ActivityManager::ActivityManagerNew())
{
    // TODO: prob need to do something more sophisticated with source_code

    if (test_file_format == Engine::yaml) {
        _test_file_reader = YamlTestFileReader::YamlTestFileReaderNew("name");   
    }
}

Test::Ptr
Engine::test_new(const std::string& test_filename)
{
    if (_test[test_filename]) {
        throw Fwk::NameInUseException("Already have \"" + test_filename + "\"");
    }

    Test::Ptr test;
    
    try {
        test = _test_file_reader->test_new(test_filename);
    } catch (Fwk::Exception e) {
        // TODO: fix what we're catching here
    }

    test->source_code_is(_source_code);

    // build up an execution activity
    Activity::Ptr execution_activity = _activity_manager->activity_new(test->name());
    execution_activity->last_notifiee_is(new ExecutionActivityReactor(execution_activity.ptr(), _activity_manager, test));
    execution_activity->status_is(Activity::waiting);

    _test[test_filename] = test;

    return test;
}

void
Engine::test_del(const std::string& name)
{
    std::map<std::string, Test::Ptr>::iterator it = _test.find(name);
    if (it == _test.end()) return;

    _test.erase(it);
}

} /* end namespace */