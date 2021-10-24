#include <string>
#include <vector>
#include <map>
#include "util.h"
#include "cpputil.h"
#include "grammar_symbols.h"
#include "node.h"
#include "ast.h"

const char *ast_get_tag_name(int ast_tag) {
  if (ast_tag < AST_PROGRAM) {
    // is actually a terminal or nonterminal symbol
    return get_grammar_symbol_name(ast_tag);
  }

  switch (ast_tag) {
  case AST_PROGRAM: return "program";
  case AST_DECLARATIONS: return "declarations";
  case AST_CONSTANT_DECLARATIONS: return "constant_declarations";
  case AST_CONSTANT_DEF: return "constant_def";
  case AST_TYPE_DECLARATIONS: return "type_declarations";
  case AST_TYPE_DEF: return "type_def";
  case AST_NAMED_TYPE: return "named_type";
  case AST_ARRAY_TYPE: return "array_type";
  case AST_RECORD_TYPE: return "record_type";
  case AST_VAR_DECLARATIONS: return "var_declarations";
  case AST_VAR_DEF: return "var_def";
  case AST_ADD: return "add";
  case AST_SUBTRACT: return "subtract";
  case AST_MULTIPLY: return "multiply";
  case AST_DIVIDE: return "divide";
  case AST_MODULUS: return "modulus";
  case AST_NEGATE: return "negate";
  case AST_INT_LITERAL: return "int_literal";
  case AST_INSTRUCTIONS: return "instructions";
  case AST_ASSIGN: return "assign";
  case AST_IF: return "if";
  case AST_IF_ELSE: return "if_else";
  case AST_REPEAT: return "repeat";
  case AST_WHILE: return "while";
  case AST_COMPARE_EQ: return "compare_eq";
  case AST_COMPARE_NEQ: return "compare_neq";
  case AST_COMPARE_LT: return "compare_lt";
  case AST_COMPARE_LTE: return "compare_lte";
  case AST_COMPARE_GT: return "compare_gt";
  case AST_COMPARE_GTE: return "compare_gte";
  case AST_WRITE: return "write";
  case AST_READ: return "read";
  case AST_VAR_REF: return "var_ref";
  case AST_ARRAY_ELEMENT_REF: return "array_element_ref";
  case AST_FIELD_REF: return "field_ref";
  case AST_IDENTIFIER_LIST: return "identifier_list";
  case AST_EXPRESSION_LIST: return "expression_list";
  default:
    err_fatal("Unknown AST node type %d\n", ast_tag);
    return "<<unknown>>";
  }
}

class ASTGraphPrinter {
private:
  std::map<std::string, int> m_node_levels;
  std::map<std::string, std::vector<std::string>> m_edges;
  std::map<int, int> m_node_type_count;
  struct Node *m_ast;

public:
  ASTGraphPrinter(struct Node *ast);
  ~ASTGraphPrinter();

  void print();

private:
  // returns maximum level
  int visit(struct Node *n, const std::string &parent_name, int level);
};

ASTGraphPrinter::ASTGraphPrinter(struct Node *ast)
  : m_ast(ast) {
}

ASTGraphPrinter::~ASTGraphPrinter() {
}

void ASTGraphPrinter::print() {
  visit(m_ast, "", 0);

  printf("digraph ast {\n");
  printf("  graph [ordering=\"out\"];\n");

  // output ranks so nodes are at the correct heights
  for (std::map<std::string, int>::iterator i = m_node_levels.begin(); i != m_node_levels.end(); i++) {
    std::string rankattr = cpputil::format("{ rank = %d; \"%s\"; }", i->second, i->first.c_str());
    printf("  %s\n", rankattr.c_str());
  }

  // output edges
  for (std::map<std::string, std::vector<std::string>>::iterator i = m_edges.begin(); i != m_edges.end(); i++) {
    for (std::vector<std::string>::iterator j = i->second.begin(); j != i->second.end(); j++) {
      std::string edge = cpputil::format("\"%s\" -> \"%s\";", i->first.c_str(), j->c_str());
      printf("  %s\n", edge.c_str());
    }
  }

  printf("}\n");
}

int ASTGraphPrinter::visit(struct Node *n, const std::string &parent_name, int level) {
  int tag = node_get_tag(n);
  std::map<int, int>::iterator i = m_node_type_count.find(tag);
  int count = (i == m_node_type_count.end()) ? 0 : i->second;
  m_node_type_count[tag] = count + 1;
  std::string tag_name = ast_get_tag_name(tag);
  if (tag_name == "TOK_IDENT") {
    tag_name = "identifier";
  }
  std::string node_name = cpputil::format("%s_%d", tag_name.c_str(), count);
  const char *strval = node_get_str(n);
  if (strval) {
    node_name += "\\n[";
    node_name += strval;
    node_name += "]";
  }
  m_node_levels[node_name] = level;
  if (!parent_name.empty()) {
    m_edges[parent_name].push_back(node_name);
  }
  int num_kids = node_get_num_kids(n);
  int max_level = level;
  for (int j = 0; j < num_kids; j++) {
    int subtree_level = visit(node_get_kid(n, j), node_name, level + 1);
    if (subtree_level > max_level) {
      max_level = subtree_level;
    }
  }
  return max_level;
}

void ast_print_graph(struct Node *ast) {
  ASTGraphPrinter agp(ast);
  agp.print();
}
