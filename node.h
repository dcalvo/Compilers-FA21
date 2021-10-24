#ifndef NODE_H
#define NODE_H

#ifdef __cplusplus

#include <string>
#include <vector>

#endif // __cplusplus

// Node datatype and functions

// Forward declaration for SymbolTable, SymbolTableEntry, Type
struct SymbolTable;
struct Symbol;
struct Type;

// Source information structure.
struct SourceInfo {
  const char *filename;
  int line, col;
};

struct Node;

#ifdef __cplusplus

// Node data type exposed as a full C++ class.
// The C functions from previous assignments still work,
// and are retained for backwards compatibility.
struct Node {
private:
  int m_tag;
  std::vector<Node *> m_kids;
  SourceInfo m_source_info;
  long m_ival;
  std::string m_strval;
/*
  SymbolTable *m_symtab;
  unsigned m_index; // index of symbol table entry
  Type *m_type;
*/

  // copy ctor and assignment operator disallowed
  Node(const Node &);
  Node &operator=(const Node &);
  
public:
  typedef std::vector<Node *>::iterator iterator;

  Node(int tag);
  ~Node();

  iterator begin() { return m_kids.begin(); }
  iterator end() { return m_kids.end(); }

  int get_tag() const;
  int get_num_kids() const;
  void add_kid(Node *kid);
  void prepend_kid(Node *kid);
  Node *get_kid(int index);
  void set_str(const std::string &s);
  const std::string &get_str() const;
  SourceInfo get_source_info() const;
  void set_source_info(const SourceInfo &source_info);
  long get_ival() const;
  void set_ival(long ival);
/*
  void set_symtab(SymbolTable *symtab);
  SymbolTable *get_symtab();
  void set_index(unsigned index);
  unsigned get_index() const;
  void set_symbol(SymbolTable *symtab, unsigned index);
  void set_type(Type *type);
  Type *get_type();
*/
};

extern "C" {
#endif // __cplusplus

// C functions for working with Nodes.
// These can be called from C code.

// Create Node with specified tag.
struct Node *node_alloc(int tag);

// Create a node with a string value by copying a specified string.
struct Node *node_alloc_str_copy(int tag, const char *str_to_copy);

// Create a node with a string value by adopting specific string,
// which will be freed when the Node is destroyed.
struct Node *node_alloc_str_adopt(int tag, char *str_to_adopt);

// Create a node with a given integer value.
struct Node *node_alloc_ival(int tag, long ival);

// Convenience functions to create a Node with specified tag value
// and pointers to children.
struct Node *node_build0(int tag);
struct Node *node_build1(int tag, struct Node *child1);
struct Node *node_build2(int tag, struct Node *child1, struct Node *child2);
struct Node *node_build3(int tag, struct Node *child1, struct Node *child2, struct Node *child3);
struct Node *node_build4(int tag,
                         struct Node *child1, struct Node *child2, struct Node *child3,
                         struct Node *child4);
struct Node *node_build5(int tag,
                         struct Node *child1, struct Node *child2, struct Node *child3,
                         struct Node *child4, struct Node *child5);
struct Node *node_build6(int tag,
                         struct Node *child1, struct Node *child2, struct Node *child3,
                         struct Node *child4, struct Node *child5, struct Node *child6);
struct Node *node_build7(int tag,
                         struct Node *child1, struct Node *child2, struct Node *child3,
                         struct Node *child4, struct Node *child5, struct Node *child6,
                         struct Node *child7);
struct Node *node_build8(int tag,
                         struct Node *child1, struct Node *child2, struct Node *child3,
                         struct Node *child4, struct Node *child5, struct Node *child6,
                         struct Node *child7, struct Node *child8);

// Create a Node with specified tag and arbitrary pointers to children.
// The sequence of child pointers should be terminated with a null pointer.
struct Node *node_buildn(int tag, ...);

// Destroy a Node.
void node_destroy(struct Node *n);

// Recursively destroy a tree of Nodes.
void node_destroy_recursive(struct Node *n);

// Get a Node's tag.
int node_get_tag(struct Node *n);

// Get the number of children of a Node.
int node_get_num_kids(struct Node *n);

// Append a child Node.
void node_add_kid(struct Node *n, struct Node *kid);

// Prepend a child Node.
void node_prepend_kid(struct Node *n, struct Node *kid);

// Get a child Node (index 0 is the first child.)
struct Node *node_get_kid(struct Node *n, int index);
int node_first_kid_has_tag(struct Node *n, int tag);

// Set the SourceInfo for a Node.
void node_set_source_info(struct Node *n, struct SourceInfo source_info);

// Get the SourceInfo for a Node.
struct SourceInfo node_get_source_info(struct Node *n);

// Get the string value of a Node.
const char *node_get_str(struct Node *n);

// Get integer value of a Node.
long node_get_ival(struct Node *n);

// Set integer value of a Node.
void node_set_ival(struct Node *n, long ival);

/*
// Set symbol table, entry
void node_set_symbol(struct Node *n, struct SymbolTable *symtab, unsigned index);

// Get symbol table
struct SymbolTable *node_get_symtab(struct Node *n);

// Get symbol index
unsigned node_get_index(struct Node *n);

// Get Symbol table entry
struct Symbol *node_get_symbol(struct Node *n);
*/

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // NODE_H
