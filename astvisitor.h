#ifndef ASTVISITOR_H
#define ASTVISITOR_H

class ASTVisitor {
public:
  ASTVisitor();
  virtual ~ASTVisitor();

  void visit(struct Node *ast);

  virtual void visit_program(struct Node *ast);
  virtual void visit_declarations(struct Node *ast);
  virtual void visit_constant_declarations(struct Node *ast);
  virtual void visit_constant_def(struct Node *ast);
  virtual void visit_type_declarations(struct Node *ast);
  virtual void visit_type_def(struct Node *ast);
  virtual void visit_named_type(struct Node *ast);
  virtual void visit_array_type(struct Node *ast);
  virtual void visit_record_type(struct Node *ast);
  virtual void visit_var_declarations(struct Node *ast);
  virtual void visit_var_def(struct Node *ast);
  virtual void visit_add(struct Node *ast);
  virtual void visit_subtract(struct Node *ast);
  virtual void visit_multiply(struct Node *ast);
  virtual void visit_divide(struct Node *ast);
  virtual void visit_modulus(struct Node *ast);
  virtual void visit_negate(struct Node *ast);
  virtual void visit_int_literal(struct Node *ast);
  virtual void visit_instructions(struct Node *ast);
  virtual void visit_assign(struct Node *ast);
  virtual void visit_if(struct Node *ast);
  virtual void visit_if_else(struct Node *ast);
  virtual void visit_repeat(struct Node *ast);
  virtual void visit_while(struct Node *ast);
  virtual void visit_compare_eq(struct Node *ast);
  virtual void visit_compare_neq(struct Node *ast);
  virtual void visit_compare_lt(struct Node *ast);
  virtual void visit_compare_lte(struct Node *ast);
  virtual void visit_compare_gt(struct Node *ast);
  virtual void visit_compare_gte(struct Node *ast);
  virtual void visit_write(struct Node *ast);
  virtual void visit_read(struct Node *ast);
  virtual void visit_var_ref(struct Node *ast);
  virtual void visit_array_element_ref(struct Node *ast);
  virtual void visit_field_ref(struct Node *ast);
  virtual void visit_identifier_list(struct Node *ast);
  virtual void visit_expression_list(struct Node *ast);
  virtual void visit_identifier(struct Node *ast);

  virtual void recur_on_children(struct Node *ast);
};

#endif // ASTVISITOR_H
