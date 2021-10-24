#include <cstdio>
#include <cstdarg>
#include <cassert>
#include "util.h"
/*
#include "symbol.h"
*/
#include "node.h"

#define DEBUG_PRINT(args...)
//#define DEBUG_PRINT(args...) printf(args)

////////////////////////////////////////////////////////////////////////
// C++ Node data type implementation
////////////////////////////////////////////////////////////////////////

Node::Node(int tag)
  : m_tag(tag)
  , m_source_info { .filename = "<unknown file>", .line = -1, .col = -1 }
  , m_ival(0L)
/*
  , m_symtab(nullptr)
  , m_index(0)
*/ {
}

Node::~Node() {
}

int Node::get_tag() const {
  return m_tag;
}

int Node::get_num_kids() const {
  return int(m_kids.size());
}

void Node::add_kid(Node *kid) {
  m_kids.push_back(kid);

  // If the parent node doesn't yet have source info set,
  // and the child has source info, copy the child's source info
  if (m_source_info.line < 0 && kid->m_source_info.line > 0) {
    m_source_info = kid->m_source_info;
  }
}

void Node::prepend_kid(Node *kid) {
  m_kids.insert(m_kids.begin(), kid);

  // Copy child's source info, if it has valid source info
  if (kid->m_source_info.line > 0) {
    m_source_info = kid->m_source_info;
  }
}

Node *Node::get_kid(int index) {
  assert(index >= 0);
  return m_kids.at(unsigned(index));
}

void Node::set_str(const std::string &s) {
  m_strval = s;
}

const std::string &Node::get_str() const {
  return m_strval;
}

SourceInfo Node::get_source_info() const {
  return m_source_info;
}

void Node::set_source_info(const SourceInfo &source_info) {
  m_source_info = source_info;
}

long Node::get_ival() const {
  return m_ival;
}

void Node::set_ival(long ival) {
  m_ival = ival;
}

/*
void Node::set_symtab(SymbolTable *symtab) {
  m_symtab = symtab;
}

SymbolTable *Node::get_symtab() {
  return m_symtab;
}

void Node::set_index(unsigned index) {
  m_index = index;
}

unsigned Node::get_index() const {
  return m_index;
}

void Node::set_symbol(SymbolTable *symtab, unsigned index) {
  m_symtab = symtab;
  m_index = index;
}

void Node::set_type(Type *type) {
  m_type = type;
}

Type *Node::get_type() {
  // If the Node is directly annotated with a Type,
  // return it
  if (m_type) {
    return m_type;
  }

  // If there is a symbol table entry, then return its type
  Symbol *sym = node_get_symbol(this);
  if (sym) {
    return sym->get_type();
  }

  // no type is associated with this node
  return nullptr;
}
*/

////////////////////////////////////////////////////////////////////////
// C API for working with Nodes
////////////////////////////////////////////////////////////////////////

struct Node *node_alloc(int tag) {
  return new Node(tag);
}

struct Node *node_alloc_str_copy(int tag, const char *str_to_copy) {
  Node *n = new Node(tag);
  n->set_str(str_to_copy);
  return n;
}

struct Node *node_alloc_str_adopt(int tag, char *str_to_adopt) {
  Node *n = new Node(tag);
  n->set_str(str_to_adopt);
  free(str_to_adopt);
  return n;
}

struct Node *node_alloc_ival(int tag, long ival) {
  Node *n = new Node(tag);
  n->set_ival(ival);
  return n;
}

struct Node *node_build0(int tag) {
  DEBUG_PRINT("Node0: %d\n", tag);
  return node_buildn(tag, NULL);
}

struct Node *node_build1(int tag, struct Node *child1) {
  DEBUG_PRINT("Node1: %d\n", tag);
  return node_buildn(tag, child1, NULL);
}

struct Node *node_build2(int tag, struct Node *child1, struct Node *child2) {
  DEBUG_PRINT("Node2: %d\n", tag);
  return node_buildn(tag, child1, child2, NULL);
}

struct Node *node_build3(int tag, struct Node *child1, struct Node *child2, struct Node *child3) {
  DEBUG_PRINT("Node3: %d\n", tag);
  return node_buildn(tag, child1, child2, child3, NULL);
}

struct Node *node_build4(int tag,
                         struct Node *child1, struct Node *child2, struct Node *child3,
                         struct Node *child4) {
  DEBUG_PRINT("Node4: %d\n", tag);
  return node_buildn(tag, child1, child2, child3, child4, NULL);
}

struct Node *node_build5(int tag,
                         struct Node *child1, struct Node *child2, struct Node *child3,
                         struct Node *child4, struct Node *child5) {
  DEBUG_PRINT("Node5: %d\n", tag);
  return node_buildn(tag, child1, child2, child3, child4, child5, NULL);
}

struct Node *node_build6(int tag,
                         struct Node *child1, struct Node *child2, struct Node *child3,
                         struct Node *child4, struct Node *child5, struct Node *child6) {
  DEBUG_PRINT("Node6: %d\n", tag);
  return node_buildn(tag, child1, child2, child3, child4, child5, child6, NULL);
}

struct Node *node_build7(int tag,
                         struct Node *child1, struct Node *child2, struct Node *child3,
                         struct Node *child4, struct Node *child5, struct Node *child6,
                         struct Node *child7) {
  DEBUG_PRINT("Node7: %d\n", tag);
  return node_buildn(tag, child1, child2, child3, child4, child5, child6, child7, NULL);
}

struct Node *node_build8(int tag,
                         struct Node *child1, struct Node *child2, struct Node *child3,
                         struct Node *child4, struct Node *child5, struct Node *child6,
                         struct Node *child7, struct Node *child8) {
  DEBUG_PRINT("Node8: %d\n", tag);
  return node_buildn(tag, child1, child2, child3, child4, child5, child6, child7, child8, NULL);
}

struct Node *node_buildn(int tag, ...) {
  va_list args;
  va_start(args, tag);
  struct Node *n = node_alloc(tag);
  int done = 0;
  while (!done) {
    struct Node *child = (struct Node *) va_arg(args, struct Node *);
    if (child) {
      node_add_kid(n, child);
    } else {
      done = 1;
    }
  }
  va_end(args);
  return n;
}

void node_destroy(struct Node *n) {
  delete n;
}

void node_destroy_recursive(struct Node *n) {
  int num_kids = n->get_num_kids();
  for (int i = 0; i < num_kids; i++) {
    Node *kid = n->get_kid(i);
    if (kid) {
      node_destroy_recursive(kid);
    }
  }
  node_destroy(n);
}

int node_get_tag(struct Node *n) {
  return n->get_tag();
}

int node_get_num_kids(struct Node *n) {
  return n->get_num_kids();
}

void node_add_kid(struct Node *n, struct Node *kid) {
  n->add_kid(kid);
}

void node_prepend_kid(struct Node *n, struct Node *kid) {
  n->prepend_kid(kid);
}

struct Node *node_get_kid(struct Node *n, int index) {
  return n->get_kid(index);
}

int node_first_kid_has_tag(struct Node *n, int tag) {
  return n->get_num_kids() > 0 && n->get_kid(0)->get_tag() == tag;
}

void node_set_source_info(struct Node *n, struct SourceInfo source_info) {
  n->set_source_info(source_info);
}

struct SourceInfo node_get_source_info(struct Node *n) {
  return n->get_source_info();
}

const char *node_get_str(struct Node *n) {
  return n->get_str().c_str();
}

long node_get_ival(struct Node *n) {
  return n->get_ival();
}

void node_set_ival(struct Node *n, long ival) {
  n->set_ival(ival);
}

/*
void node_set_symbol(struct Node *n, struct SymbolTable *symtab, unsigned index) {
  n->set_symtab(symtab);
  n->set_index(index);
}

struct SymbolTable *node_get_symtab(struct Node *n) {
  return n->get_symtab();
}

unsigned node_get_index(struct Node *n) {
  return n->get_index();
}

// Assume that this function exists, and can retrieve a pointer to
// a Symbol from a SymbolTable, given its index.
extern "C" struct Symbol *symtab_get_entry(struct SymbolTable *symtab, unsigned index);

struct Symbol *node_get_symbol(struct Node *n) {
  SymbolTable *symtab = n->get_symtab();
  if (!symtab) {
    // The Node doesn't have a symbol table entry
    return nullptr;
  }
  return symtab_get_entry(symtab, n->get_index());
}
*/
