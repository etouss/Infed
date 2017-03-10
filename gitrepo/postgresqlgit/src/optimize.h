#ifndef OPTIMIZE_H
#define OPTIMIZE_H


#include "postgres.h"
#include "parser/parse_node.h"
#include "parser/analyze.h"
#include "nodes/nodeFuncs.h"
#include "nodes/makefuncs.h"

#include <ctype.h>
#include <sys/resource.h>

#include "commands/user.h"
#include "fmgr.h"
#include "libpq/md5.h"

#include "access/hash.h"
#include "access/htup_details.h"
#include "access/nbtree.h"
#include "bootstrap/bootstrap.h"
#include "catalog/namespace.h"
#include "catalog/pg_am.h"
#include "catalog/pg_amop.h"
#include "catalog/pg_amproc.h"
#include "catalog/pg_collation.h"
#include "catalog/pg_constraint.h"
#include "catalog/pg_language.h"
#include "catalog/pg_namespace.h"
#include "catalog/pg_opclass.h"
#include "catalog/pg_operator.h"
#include "catalog/pg_proc.h"
#include "catalog/pg_range.h"
#include "catalog/pg_statistic.h"
#include "catalog/pg_transform.h"
#include "catalog/pg_type.h"
#include "miscadmin.h"
#include "nodes/makefuncs.h"
#include "utils/array.h"
#include "utils/builtins.h"
#include "utils/catcache.h"
#include "utils/datum.h"
#include "utils/fmgroids.h"
#include "utils/lsyscache.h"
#include "utils/rel.h"
#include "utils/syscache.h"
#include "utils/typcache.h"

#include "true_var.h"


typedef struct context_optimizer_self {
  bool disjunction;
  bool equality;
  bool add;
  // bool ready;
  // List * constraint_to_add;
  List * already_constrain_rel;
  int varno_to_add;
  int varlevelsup_to_add;
  int varno_to_add_or;
  int varlevelsup_to_add_or;
  int varno_to_add_eq;
  int varlevelsup_to_add_eq;
  // List * list_of_not_null_in_current;
  // List * list_of_not_null_att_schem;
  // List * list_list_true;
  // List * list_list_false;

  // int where_i_am;
  // int where_i_am_querry;
  // int current_varlevelsup;

}context_optimizer_self;

typedef struct temp_context_exists {
  List * composante_connexe_sub_level;
  List * list_composantes_connexes;
  int varno_en_cours;
  bool var_sub;
}temp_context_exists;


context_optimizer_self* make_context_optimizer_self();
void free_context_optimizer_self(context_optimizer_self * context);

Node * my_optimize_self (Node *node, context_optimizer_self *context);
Node * my_modifier_self(Node *node, context_optimizer_self *context);
Node * rewrite_self(Query *q, List *constraint_to_add);
List * trueSplit(Node * negNode);
bool create_connexe(Node * node, temp_context_exists * ctx);
temp_context_exists * make_temp_context_exists(int nb_varno);





#endif
