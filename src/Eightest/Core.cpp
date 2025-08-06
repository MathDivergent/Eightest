#include <cstdio> // printf

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

static void update_stat(unsigned& passed, unsigned& failed, bool ok)
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

void registry_t::execute_module(std::string const& name)
{
    auto it = all.find(name);
    if (it == all.end()) return;

    auto& module = it->second;
    for (auto& name_test : module) name_test.second->run();
}

void registry_t::execute_test(std::string const& name)
{
    for (auto& name_module : all)
    {
        auto& module = name_module.second;

        auto it = module.find(name);
        if (it == module.end()) continue;

        it->second->run();
    }
}

void registry_t::execute_all()
{
    for (auto& name_module : all)
    {
        auto& module = name_module.second;
        for (auto& name_test : module) name_test.second->run();
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

void registry_t::try_catch(std::function<void()> const& call) const noexcept
{
    try { call(); }
    catch(char const* e) { stat_handler(e); }
    catch(std::exception& e) { stat_handler(e.what()); }
    catch(...) { stat_handler("Unexpected error."); }
}

void registry_t::default_stat_handler(std::string const& stat)
{
    printf("%s", stat.c_str());
}

registry_t* global()
{
    static registry_t self; return &self;
}

} // namespace eightest
