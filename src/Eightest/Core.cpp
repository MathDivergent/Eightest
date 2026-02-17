#ifdef EIGHTEST_DEFAULT_STAT_HANDLER
#include <cstdio> // printf
#endif // EIGHTEST_DEFAULT_STAT_HANDLER

#include <Eightest/Core.hpp>

namespace eightest
{

expression_t<std::nullptr_t> expression(std::nullptr_t expression_value)
{
    return expression_t<std::nullptr_t>{expression_value, "nullptr"};
}

expression_t<bool> expression(bool expression_value)
{
    return expression_t<bool>{expression_value, expression_value == true ? "true" : "false"};
}

test_t::test_t(std::string const& module, std::string const& name)
    : module(module), name(name)
{
}

void test_t::bind(registry_t* to)
{
    registry = to;

    if (registry == nullptr)
    {
        registry = global();
    }

    registry->add(this);
}

static std::string info_format(test_t* test, std::string const& msg, std::string const& string_value, bool ok)
{
    return (ok ? "[   OK   ] " : "[ FAILED ] ")
         + std::string(test->module)
         + "::"
         + test->name
         + '.'
         + msg
         + (ok ? "\n" : " [  WITH  ] " + string_value + "\n");
}

static void update_stat(std::size_t& passed, std::size_t& failed, bool ok)
{
    passed += ok;
    failed += not ok;
}

void registry_t::add(test_t* test)
{
    auto& place = all[test->module][test->name];
    if (place == nullptr)
    {
        place = test;
    }
    else
    {
        stat_handler(std::string("[ COPIED ]") + test->module + "::" + test->name);
        failed += 1;
    }
}

bool registry_t::check(bool condition, test_t* test, std::string const& msg)
{
    return check(expression(condition), test, msg);
}

bool registry_t::check(expression_t<bool> const& expression, test_t* test, std::string const& msg)
{
    auto const condition = static_cast<bool>(expression.value);
    update_stat(passed, failed, condition);
    stat_handler(info_format(test, msg, expression.string_value, condition));
    return condition;
}

void registry_t::safe_run(test_t* test)
{
    try
    {
        test->run();
    }
    catch(char const* e)
    {
        stat_handler(info_format(test, "<exception>", e, false));
    }
    catch(std::exception const& e)
    {
        stat_handler(info_format(test, "<exception>", e.what(), false));
    }
    catch(...)
    {
        stat_handler(info_format(test, "<exception>", "<unknown>", false));
    }
}

void registry_t::execute_module(std::string const& name)
{
    auto it = all.find(name);
    if (it == all.end()) return;

    auto& module = it->second;
    for (auto& name_test : module) safe_run(name_test.second);
}

void registry_t::execute_test(std::string const& name)
{
    for (auto& name_module : all)
    {
        auto& module = name_module.second;

        auto it = module.find(name);
        if (it == module.end()) continue;

        safe_run(it->second);
    }
}

void registry_t::execute_all()
{
    for (auto& name_module : all)
    {
        auto& module = name_module.second;
        for (auto& name_test : module) safe_run(name_test.second);
    }
}

static std::string stat_format(unsigned passed, unsigned failed)
{
    return "\nOK: "
         + std::to_string(passed)
         + " FAILED: "
         + std::to_string(failed)
         + " TOTAL: "
         + std::to_string(passed+failed)
         + '\n';
}

bool registry_t::stat()
{
    stat_handler(stat_format(passed, failed));
    return failed == 0;
}

void registry_t::default_stat_handler(std::string const& stat)
{
    #ifdef EIGHTEST_DEFAULT_STAT_HANDLER
    printf("%s", stat.c_str());
    #endif // EIGHTEST_DEFAULT_STAT_HANDLER
}

registry_t* global()
{
    static registry_t self; return &self;
}

} // namespace eightest
