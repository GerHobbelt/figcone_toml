#include "assert_exception.h"
#include <figcone_toml/parser.h>
#include <figcone_tree/errors.h>
#include <gtest/gtest.h>
#include <sstream>

namespace test_nodeparser {

auto parse(const std::string& str)
{
    auto input = std::stringstream{str};
    auto parser = figcone::toml::Parser{};
    return parser.parse(input);
}

TEST(TestNodeParser, SingleNodeSingleLevel)
{
    auto result = parse(R"(
    foo = 5
    bar = "test"
    [a]
      testInt = 10
    )");

    auto& tree = result.asItem();
    ASSERT_EQ(tree.paramsCount(), 2);
    ASSERT_EQ(tree.hasParam("foo"), 1);
    ASSERT_EQ(tree.hasParam("bar"), 1);
    EXPECT_EQ(tree.param("foo").value(), "5");
    EXPECT_EQ(tree.param("bar").value(), "test");
    ASSERT_EQ(tree.nodesCount(), 1);
    ASSERT_EQ(tree.hasNode("a"), 1);
    auto& aNode = tree.node("a").asItem();
    ASSERT_EQ(aNode.paramsCount(), 1);
    EXPECT_EQ(aNode.param("testInt").value(), "10");
}

TEST(TestNodeParser, MultiNodeSingleLevel)
{

    auto result = parse(R"(
    foo = 5
    bar = "test"
    [a]
      testInt = 10
    [b]
      testInt = 11
    )");

    auto& tree = result.asItem();
    ASSERT_EQ(tree.paramsCount(), 2);
    ASSERT_EQ(tree.hasParam("foo"), 1);
    ASSERT_EQ(tree.hasParam("bar"), 1);
    EXPECT_EQ(tree.param("foo").value(), "5");
    EXPECT_EQ(tree.param("bar").value(), "test");
    ASSERT_EQ(tree.nodesCount(), 2);
    ASSERT_EQ(tree.hasNode("a"), 1);
    ASSERT_EQ(tree.hasNode("b"), 1);
    auto& aNode = tree.node("a").asItem();
    ASSERT_EQ(aNode.paramsCount(), 1);
    EXPECT_EQ(aNode.param("testInt").value(), "10");
    auto& bNode = tree.node("b").asItem();
    ASSERT_EQ(bNode.paramsCount(), 1);
    EXPECT_EQ(bNode.param("testInt").value(), "11");
}

TEST(TestNodeParser, MultiLevel)
{
    auto result = parse(R"(
    foo = 5
    bar = "test"
    [c]
      testInt = 11
      testDouble = 12
      [c.b]
        testInt = 10
        testString = "Hello world"
    [b]
      testInt = 9
    )");

    auto& tree = result.asItem();
    ASSERT_EQ(tree.paramsCount(), 2);
    ASSERT_EQ(tree.hasParam("foo"), 1);
    ASSERT_EQ(tree.hasParam("bar"), 1);
    EXPECT_EQ(tree.param("foo").value(), "5");
    EXPECT_EQ(tree.param("bar").value(), "test");
    ASSERT_EQ(tree.nodesCount(), 2);
    ASSERT_EQ(tree.hasNode("c"), 1);
    ASSERT_EQ(tree.hasNode("b"), 1);

    auto& cNode = tree.node("c").asItem();
    ASSERT_EQ(cNode.paramsCount(), 2);
    EXPECT_EQ(cNode.param("testInt").value(), "11");
    EXPECT_EQ(cNode.param("testDouble").value(), "12");
    ASSERT_EQ(cNode.nodesCount(), 1);
    ASSERT_EQ(cNode.hasNode("b"), 1);
    auto& cbNode = cNode.node("b").asItem();
    ASSERT_EQ(cbNode.paramsCount(), 2);
    EXPECT_EQ(cbNode.param("testInt").value(), "10");
    EXPECT_EQ(cbNode.param("testString").value(), "Hello world");

    auto& bNode = tree.node("b").asItem();
    ASSERT_EQ(bNode.paramsCount(), 1);
    EXPECT_EQ(bNode.param("testInt").value(), "9");
}

TEST(TestNodeParam, DublicateParamError)
{
    assert_exception<figcone::ConfigError>([] {
        parse(R"(
        foo = 5
        foo = "test"
        [a]
          testInt = 10
        )");
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:3, column:15] [error] toml::insert_value: value (\"foo\") already exists.\n"
                                             " --> unknown file\n"
                                             "   |\n"
                                             " 2 |         foo = 5\n"
                                             "   |               ^--- value already exists here\n"
                                             " ...\n"
                                             " 3 |         foo = \"test\"\n"
                                             "   |               ~~~~~~ value defined twice");
    });
}


}