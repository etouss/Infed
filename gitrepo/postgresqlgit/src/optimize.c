#include "optimize.h"
#include "string.h"

typedef struct temp_context {
  int varno_to_add;
  int varlevelsup_to_add;
  Node * constraint_to_add;
  bool to_add;
  bool already_add;
}temp_context;

typedef struct temp_context_rewrite {
  int varno_to_add;
  int varlevelsup_to_add;
  bool to_add;
}temp_context_rewrite;

temp_context_rewrite * make_temp_context_rewrite(){
  temp_context_rewrite * ctx = palloc(sizeof(temp_context_rewrite));
  ctx->varno_to_add = -1;
  ctx->varlevelsup_to_add = -1;
  ctx->to_add = false;
  return ctx;
}

typedef struct temp_context_split {
  int which_i_choose;
  bool have_to_duplicate;
  bool stop;
}temp_context_split;

temp_context_split * make_temp_context_split(){
  temp_context_split * ctx = palloc(sizeof(temp_context_split));
  ctx->which_i_choose = 0;
  ctx->have_to_duplicate = true;
  ctx->stop = false;
  return ctx;
}

temp_context_exists * make_temp_context_exists(int nb_varno){
  temp_context_exists * ctx = palloc(sizeof(temp_context_exists));
  ctx->composante_connexe_sub_level = NULL;
  ctx->list_composantes_connexes = NULL;
  for(int i = 0;i<nb_varno;i++){
    List * tmp = NULL;
    tmp = lappend_int(tmp,i+1);
    ctx->list_composantes_connexes = lappend(ctx->list_composantes_connexes,tmp);
  }
  ctx->varno_en_cours = -1;
  ctx->var_sub = false;
  return ctx;
}

typedef struct temp_context_connex_rew {
  List * list_constraint;
  List * list_varno;
  bool to_remove;
}temp_context_connex_rew;

typedef struct temp_context_connex_rew2 {
  List * list_varno;
  List * rtbl;
  List * targetList;
  bool to_add;
}temp_context_connex_rew2;

temp_context_connex_rew2 * make_temp_context_connex_rew2(List * l){
  temp_context_connex_rew2 * ctx = palloc(sizeof(temp_context_connex_rew));
  ctx->list_varno = l;
  ctx->rtbl = NULL;
  ctx->targetList = NULL;
  ctx->to_add = false;
  return ctx;
}



temp_context_connex_rew * make_temp_context_connex_rew(List * l){
  temp_context_connex_rew * ctx = palloc(sizeof(temp_context_connex_rew));
  ctx->list_constraint = NULL;
  ctx->list_varno = l;
  ctx->to_remove = false;
  return ctx;
}

/* typedef struct temp_context_connex_create_q { */
/*   Node * rtable; */
/*   Node * rtarget; */
/*   bool to_remove; */
/* }temp_context_connex_rew; */


int is_in_list(int varno, List *list_varno){
  const ListCell *cell;
  int i = 1;
  foreach(cell, list_varno) 
	{
    if(varno == lfirst_int(cell))
      return i;
    i++;
  }
  return 0;
}

int deacrease(int varno, List * list_varno){
  int i = 0;
  const ListCell *cell;
  foreach(cell, list_varno) 
	{
    if(varno > lfirst_int(cell)){
      i++;
    }
  }
  return varno - i;
  
}

Node * mutator_conn(Node * node, temp_context_connex_rew * ctx){
  if (node == NULL)
    return node;
  if (IsA(node,OpExpr)){
    Node * n = expression_tree_mutator(node, mutator_conn, (void *) ctx);
    if(ctx->to_remove){
      ctx->list_constraint = lappend(ctx->list_constraint,n);
      ctx->to_remove = false;
      return NULL;
    }
    return n;
  }
  if(IsA(node,Var)){
    Var * v = (Var *) node;
    /* elog(LOG,"HERE WE ARE %s \n",nodeToString(ctx->list_varno)); */
    if(v->varlevelsup == 0 && is_in_list(v->varno,ctx->list_varno)){
        ctx->to_remove = true;
    }
    /* v->varno = deacrease(v->varno,ctx->list_varno); */
  }
  return expression_tree_mutator(node, mutator_conn, (void *) ctx);
}

Node * mutator_rew(Node * node, temp_context_connex_rew2 * ctx){

  if (node == NULL)
    return NULL;
  if(IsA(node, Var)){
    Var *v = (Var *)copyObject(node);
    int i = is_in_list(v->varno,ctx->list_varno);
    if(!i)
      return NULL;
    v->varno = i;
    ctx->to_add = true;
    return v;
  }

  if(IsA(node,List)){
    List * result = NULL;
    const ListCell *cell;
    int i = 1;
    foreach(cell, (List *)node)
	  { 
      Node * t = copyObject(lfirst(cell));
      TargetEntry * n = mutator_rew(t,ctx);
      if(ctx->to_add == true){
        n->resno = i;
        i++;
        result = lappend(result,n);
        ctx->to_add = false;
      }
    }
    /* i = 1; */
    return result;
  }
  return expression_tree_mutator(node, mutator_rew, (void *)ctx);
    
}

Node * mutator_rew3 (Node * node, temp_context_connex_rew2 * ctx){
  if (node == NULL)
    return NULL;
   if(IsA(node, Var)){
    Var *v = (Var *)copyObject(node);
    v->varno = deacrease(v->varno,ctx->list_varno);
    return v;
  }

  return expression_tree_mutator(node, mutator_rew3, (void *)ctx);
}

Node * mutator_rew2(Node * node, temp_context_connex_rew2 * ctx){

  if (node == NULL)
    return NULL;
  if(IsA(node, Var)){
    Var *v = (Var *)copyObject(node);
    int i = is_in_list(v->varno,ctx->list_varno);
    if(i)
      return NULL;
    v->varno = deacrease(v->varno,ctx->list_varno);
    ctx->to_add = true;
    return v;
  }

  if(IsA(node,List)){
    List * result = NULL;
    const ListCell *cell;
    int i = 1;
    foreach(cell, (List *)node)
	  { 
      Node * t = copyObject(lfirst(cell));
      TargetEntry * n = mutator_rew2(t,ctx);
      if(ctx->to_add == true){
        n->resno = i;
        i++;
        result = lappend(result,n);
        ctx->to_add = false;
      }
    }
    /* i = 1; */
    return result;
  }
  return expression_tree_mutator(node, mutator_rew2, (void *)ctx);
    
}


Node * create_from_query_list(List * list_rte, Node * constraint, List * listTargetEntry){
  elog(LOG,"HERE 1 \n");  
  Query * q = makeNode(Query);
  /* elog(LOG,"HERE 1 \n");   */
  q->commandType = 1;
  q->canSetTag = true;
  q->hasAggs = false;
  q->hasWindowFuncs = false;
  q->hasSubLinks = false;
  q->hasDistinctOn = false;
  q->hasRecursive = false;
  q->hasModifyingCTE = false;
  q->hasForUpdate = false;
  q->hasRowSecurity = false;
  /* elog(LOG,"HERE 1 \n"); */
  q->targetList = listTargetEntry;
  q->rtable = list_rte;

  /*LIST SIZE*/
  RangeTblRef * rtr = makeNode(RangeTblRef);
  rtr->rtindex = 1;
  q->jointree = makeFromExpr(lappend(NULL,rtr),constraint);
  
  /*Steal Target Entry From Query !*/ 
  /* elog(LOG,"Query : %s\n",nodeToString(q)); */
  return q;
}


void rewrite_connexe(Query *q, List * list_varno){
  /*Capture constrainte*/
  temp_context_connex_rew * ctx = make_temp_context_connex_rew(list_varno);
  q->jointree->quals = expression_tree_mutator((Node *)q->jointree->quals, mutator_conn, (void *) ctx);

  /*create get_RangleTbl from q, get ListTargetEntry*/
  /*create subquere Exists*/
  /*Modify previous query (modify var tar et rtbl + add exists)*/

  temp_context_connex_rew2 * ctx2 = make_temp_context_connex_rew2(list_varno);
  List * n = mutator_rew((Node *)q->targetList,ctx2);
  List * lrte = NULL;
  const ListCell *cell;
  foreach(cell, list_varno)
  {
    int v = lfirst_int(cell);
    lrte =  lappend(lrte,list_nth(q->rtable,v - 1));
  }
  SubLink * ex = makeNode(SubLink);
  ex->subselect = create_from_query_list(lrte,(Node *)ctx->list_constraint,n);
  /* elog(LOG,"QUERERE :: %s \n",nodeToString(create_from_query_list(lrte,(Node *)ctx->list_constraint,n))); */
  /* elog(LOG,"n ::: %s \n",nodeToString(n)); */
  int i = 0;
  foreach(cell, list_varno)
  { 
    int v = lfirst_int(cell);
    q->rtable = list_delete_ptr(q->rtable,list_nth(q->rtable,v - 1-i));
    i++;
  }
  q->targetList = mutator_rew2((Node *)q->targetList,ctx2);
  q->jointree = mutator_rew3((Node *)q->jointree,ctx2);
  
  /* elog(LOG,"QUE ::: %s \n",nodeToString(q)); */
  elog(LOG,"EX ::: %s \n",nodeToString(ex));



  /*modify rtarget*/


  

  

  /* Node * create_from_query_list(List * list_rte, Node * constraint, List * listTargetEntry) */

  /* elog(LOG,"TEST :::: %s \n",nodeToString(q->jointree->quals)); */
  /* elog(LOG,"TEST2 :::%s \n",nodeToString(ctx->list_constraint)); */
  /*Delete from REL it tbleRangeRld and target*/
  /*Rename var accordingly*/
  /*Add EXISTS Querry*/
}

bool is_sub(int varno,temp_context_exists * ctx){
  const ListCell *cell;
  foreach(cell, ctx->composante_connexe_sub_level)
  {
    int v = lfirst_int(cell);
    if(v == varno){
      return true;
    }
  }
  return false;

}

void remove_from_list(int varno,temp_context_exists * ctx){
  List * result = NULL;
  const ListCell *cell;
  foreach(cell, ctx->list_composantes_connexes) 
	{
    bool to_add = true;
    List * compo = lfirst(cell);
    const ListCell *cell_b;
    foreach(cell_b, compo){
      int v = lfirst_int(cell_b);
      if(v == varno){
        to_add = false;
        break;
      }
    }
    if(to_add){
      result = lappend(result,compo);
    }
  }
  ctx->list_composantes_connexes = result;
}

List * return_list(int varno,temp_context_exists * ctx){
  const ListCell *cell;
  foreach(cell, ctx->list_composantes_connexes) 
	{
    List * compo = lfirst(cell);
    const ListCell *cell_b;
    foreach(cell_b, compo){
      int v = lfirst_int(cell_b);
      if(v == varno){
        return compo;
      }
    }
  }
  foreach(cell, ctx->composante_connexe_sub_level)
  {
    int v = lfirst_int(cell);
    if(v == varno){
      return ctx->composante_connexe_sub_level;
    }
  }
  /* List * result = NULL; */
  /* result =  lappend_int(result,varno); */
  /* ctx->list_composantes_connexes = lappend(ctx->list_composantes_connexes,result); */
  return NULL;
}

bool create_connexe(Node * node, temp_context_exists * ctx){
  if (node == NULL)
    return false;
  if(IsA(node, OpExpr)){
    bool retour = expression_tree_walker(node,create_connexe,(void *)ctx);
    ctx->varno_en_cours = -1;
    ctx->var_sub = false;
    return retour;
  }
  if(IsA(node,Var)){
    Var * v = (Var *)node;
    if(ctx->varno_en_cours == -1){
      ctx->varno_en_cours = v->varno;
      if(v->varlevelsup > 0 || is_sub(v->varno,ctx))
        ctx->var_sub = true;
      return false;
    }
    bool sub = is_sub(v->varno,ctx);
    if(ctx->var_sub && (sub || v->varlevelsup > 0))
      return false;
    if(ctx->var_sub && !sub && v->varlevelsup == 0 ){ 
      List * b = return_list(v->varno,ctx);
      ctx->composante_connexe_sub_level = list_concat(ctx->composante_connexe_sub_level,b);
      remove_from_list(v->varno,ctx);
      /*Supprime b*/
      return false;
    }

    if(sub || v->varlevelsup > 0){
      List * a = return_list(ctx->varno_en_cours,ctx);
      ctx->composante_connexe_sub_level = list_concat(ctx->composante_connexe_sub_level,a);
      remove_from_list(ctx->varno_en_cours,ctx);
      return false;
      /*Remove a*/
    }
    List * a = return_list(ctx->varno_en_cours,ctx);
    List * b = return_list(v->varno,ctx);
    if(a == b) return false;
    remove_from_list(ctx->varno_en_cours,ctx);
    remove_from_list(v->varno,ctx);
    ctx->list_composantes_connexes = lappend(ctx->list_composantes_connexes,list_concat(a,b));
    return false;
  }
  return expression_tree_walker(node,create_connexe,(void *)ctx);
}




temp_context * make_temp_context(Node * constraint_to_add,int varno_to_add,int varlevelsup_to_add){
  temp_context * ctx = palloc(sizeof(temp_context));
  ctx->varno_to_add = varno_to_add;
  ctx->varlevelsup_to_add = varlevelsup_to_add;
  ctx->constraint_to_add = constraint_to_add;
  ctx->to_add = false;
  ctx->already_add = false;
  return ctx;
}

bool have_to_split(Node *node , void * context){
  if (node == NULL)
    return false;
   if(IsA(node, BoolExpr)){
     BoolExpr * bExpr =(BoolExpr *) node; 
    if(bExpr->boolop == OR_EXPR ){
      return true;
    }
   }
  return expression_tree_walker(node,have_to_split,context);
}

Node * split_rel (Node *node , temp_context_split *context){
  if (node == NULL)
    return NULL;
  if(IsA(node, BoolExpr)){
     BoolExpr * bExpr =(BoolExpr *) node; 
     if(bExpr->boolop == OR_EXPR && !context->stop){
       context->stop = true;
       List * or_args = bExpr->args;
       Node * my_arg = list_nth(or_args,context->which_i_choose);
       if(list_length(or_args) == context->which_i_choose +1){
         /*Stop duplicate*/
         context->have_to_duplicate = false;
       }
       else{
         /*Duplicate*/
         context->which_i_choose ++;
         context->have_to_duplicate = true;
       }
       /* elog(LOG,"my arg : %s \n",nodeToString(my_arg)); */
       return my_arg;
     }
    return expression_tree_mutator(node, split_rel, (void *)context);
  }
  return expression_tree_mutator(node, split_rel, (void *)context);

}

List * trueSplit(Node * negNode){
  
  List * result = NULL;
  List * pile = NULL;
  
  pile = lappend(pile,negNode);
  while(list_length(pile) != 0){
    Node * backup = list_nth(pile,0);
    pile = list_delete_first(pile);
    if(!have_to_split(backup,NULL)){
      result = lappend(result,backup);
      continue;
    }
    temp_context_split * ctx = make_temp_context_split();
    do{
      ctx->stop = false;
      Node * new_node = expression_tree_mutator(backup, split_rel, (void *)ctx);
      /* elog(LOG,"NEW NODE : %s \n",nodeToString(new_node)); */
      pile = lappend(pile,new_node);
    }while(ctx->have_to_duplicate);
  }

  elog(LOG,"CA DONNE QUOI ??? %s \n",nodeToString(result));
  return result;
}

Node * create_from_query(RangeTblEntry * rte, Node * constraint, List * listTargetEntry){
  elog(LOG,"HERE 1 \n");  
  Query * q = makeNode(Query);
  /* elog(LOG,"HERE 1 \n");   */
  q->commandType = 1;
  q->canSetTag = true;
  q->hasAggs = false;
  q->hasWindowFuncs = false;
  q->hasSubLinks = false;
  q->hasDistinctOn = false;
  q->hasRecursive = false;
  q->hasModifyingCTE = false;
  q->hasForUpdate = false;
  q->hasRowSecurity = false;
  /* elog(LOG,"HERE 1 \n"); */
  q->targetList = listTargetEntry;
  RangeTblRef * rtr = makeNode(RangeTblRef);
  rtr->rtindex = 1;
  q->jointree = makeFromExpr(lappend(NULL,rtr),constraint);
  /* const ListCell *cell; */
  /* foreach(cell, listTargetEntry) */
	/* { */
  /*   TargetEntry * t = lfirst(cell); */
  /*   if(((Var *)t->expr)->varno == 1){ */
  /*     lappend(q->targetList,(Node *)copyObject(t)); */
  /*   } */
  /* } */

  /*Steal Target Entry From Query !*/ 
  q->rtable = lappend(q->rtable,rte);
  /* elog(LOG,"Query : %s\n",nodeToString(q)); */
  return q;
}


bool init_rel(Node *node, temp_context_rewrite *context){
  if (node == NULL)
 			return false;
  if(IsA(node, Var)){
    Var *v = (Var *)node;
    context->varno_to_add = v->varno;
    context->varlevelsup_to_add = v->varlevelsup;
    return true;
  }
  return expression_tree_walker(node, init_rel, (void *) context);
}

Node * modif_rel (Node *node , void *context){
  if (node == NULL)
    return NULL;
  if(IsA(node, Var)){
    Var *v = (Var *)copyObject(node);
    v->varno = 1;
    /* context->varlevelsup_to_add = v->varlevelsup; */
    return v;
  }
  return expression_tree_mutator(node, modif_rel, context);

}

Node * modif_targetEntry(Node *node , temp_context_rewrite *context){
  /* static int i = 1; */
  if (node == NULL)
    return NULL;
  if(IsA(node, Var)){
    Var *v = (Var *)copyObject(node);
    if(v->varno != context->varno_to_add)
      return NULL;
    v->varno = 1;
    context->to_add = true;
    return v;
  }
  /* if(IsA(node, TargetEntry)){ */
  /*   TargetEntry *v = copyObject(modif_targetEntry(node,context)); */
  /*   if(context->to_add){ */
  /*     v->resno = i; */
  /*     i ++; */
  /*   } */
  /*   return v; */
  /* } */

  if(IsA(node,List)){
    List * result = NULL;
    const ListCell *cell;
    int i = 1;
    foreach(cell, (List *)node)
	  { 
      Node * t = copyObject(lfirst(cell));
      TargetEntry * n = modif_targetEntry(t,context);
      if(context->to_add == true){
        n->resno = i;
        i++;
        result = lappend(result,n);
        context->to_add = false;
      }
    }
    /* i = 1; */
    return result;
  }
  return expression_tree_mutator(node, modif_targetEntry, (void *)context);

}



Node * rewrite_self(Query *q, List *constraint_to_add){
  static int i = 0;
  const ListCell *cell;
  foreach(cell, constraint_to_add)
	{
    i++;
    Node * t = lfirst(cell);
    /* elog(LOG,"1 :: %s \n",nodeToString(t)); */
    temp_context_rewrite * ctx = make_temp_context_rewrite();
    init_rel(t,ctx);
    Node * n_constraint = modif_rel(t,NULL);
    /* elog(LOG,"2 :: %s \n",nodeToString(n_constraint)); */
    List * l = modif_targetEntry((Node *)copyObject(q->targetList),ctx);
    /* elog(LOG,"3 :: %s \n",nodeToString(l)); */
    Query * nq = create_from_query(copyObject(list_nth(q->rtable,ctx->varno_to_add - 1)),n_constraint,l);
    /* elog(LOG,"4 :: %s \n",nodeToString(nq)); */
    RangeTblEntry * ef = list_nth(q->rtable,ctx->varno_to_add - 1);
    /* elog(LOG,"test 1: %s\n",nodeToString(ef));     */
    ef->rtekind = RTE_SUBQUERY;
    /* elog(LOG,"test 2: %s\n",nodeToString(nq));     */
    ef->subquery = nq;
    /* elog(LOG,"test 3: %s\n",nodeToString(ef));     */
    char * test = malloc(sizeof(char)*20);
    sprintf(test,"alias%d",i);
    /* test[6] = 0; */
    ef->alias = makeAlias(test,NULL);
    /* elog(LOG,"test 4: %s\n",nodeToString(ef));     */
    ef->eref = makeAlias(test,ef->eref->colnames);
    /* elog(LOG,"test 5: %s\n",nodeToString(ef)); */
    /* return NULL; */
  }
return NULL;

}



Node * temp_modifier_add (Node *already_constrain_rel, temp_context *context){
  /* elog(LOG,"HERE Modifier :  %s\n",nodeToString(already_constrain_rel)); */
  if (already_constrain_rel == NULL){
    /* context->to_add = ; */
    return NULL;
  }
  if(IsA(already_constrain_rel,Var)){
    Var * v = (Var *) already_constrain_rel;
    if(v->varlevelsup == context->varlevelsup_to_add && v->varno == context->varno_to_add){
      context->to_add = true;
    }
    return v;
  }

  if(IsA(already_constrain_rel,BoolExpr)){
    Node * tmp = expression_tree_mutator(already_constrain_rel, temp_modifier_add, (void *) context);
    BoolExpr * expr = (BoolExpr *)tmp;
    if(expr->boolop == AND_EXPR && context->to_add){
        context->to_add = false;
        context->already_add = true;
        return lappend(expr->args,context->constraint_to_add);
    }
    return tmp;
  }
  return expression_tree_mutator(already_constrain_rel, temp_modifier_add, (void *) context);
}

List * add_constrain(Node *already_constrain_rel, temp_context *context){
  /* elog(LOG,"HERE Add :  %s\n",nodeToString(already_constrain_rel)); */
   if (already_constrain_rel == NULL)
     return lappend((List *)already_constrain_rel,context->constraint_to_add);

    List * result = NULL;
    List * list = (List *)already_constrain_rel;
    const ListCell *cell;
  	foreach(cell, list)
	  {
      Node * t = lfirst(cell);
      if(context->already_add){
        result = lappend(result,t);
        continue;
      }
      Node * new = temp_modifier_add(t,context);
      if(context->to_add && !context->already_add){
        context->to_add = false;
        context->already_add = true;
        List * tmp_list = NULL;
        tmp_list = lappend(tmp_list,new);
        tmp_list = lappend(tmp_list,context->constraint_to_add);
        result = lappend(result,makeBoolExpr(AND_EXPR,tmp_list,-1));
      }
      else{
        result = lappend(result,new);
      }
    }
    if(!context->already_add){
        result =  lappend(result,context->constraint_to_add);
    }
    /* list_free(list); */
    return result;
}




context_optimizer_self* make_context_optimizer_self(){
  context_optimizer_self * ctx = malloc(sizeof(context_optimizer_self));
  ctx->disjunction = false;
  ctx->equality = false;
  ctx->add = false;
  ctx->already_constrain_rel = NULL;
  ctx->varno_to_add = -1;
  ctx->varlevelsup_to_add = -1;
  ctx->varno_to_add_or = -1;
  ctx->varlevelsup_to_add_or = -1;
  ctx->varno_to_add_eq = -1;
  ctx->varlevelsup_to_add_eq = -1;

  return ctx;
}

Node * my_modifier_self(Node *node, context_optimizer_self *context){
  if (node == NULL)
 			return node;

  /* elog(LOG,"tag : %s\n",nodeToString(node)); */

  if(IsA(node,Query)){
    /* Query * q = (Query *) node; */
    /* q->jointree = expression_tree_mutator((Node *)q->jointree, my_modifier_self, (void *) context); */
    return node;
  }

  if(IsA(node,Var)){
    Var * v = (Var *)node;
    if(context->disjunction && context->varno_to_add_or != -1 && (context->varno_to_add_or != v->varno || context->varlevelsup_to_add_or != v->varlevelsup )){
      context->add = false;
      return node;
    }
    if(context->equality && context->varno_to_add_eq != -1 && (context->varno_to_add_eq != v->varno || context->varlevelsup_to_add_eq != v->varlevelsup )){
      context->add = false;
      return node;
    }
    context->varno_to_add = v->varno;
    context->varlevelsup_to_add = v->varlevelsup;
    if(context->disjunction){
      context->varno_to_add_or = v->varno;
      context->varlevelsup_to_add_or = v->varlevelsup;
    }
    if(context->equality){
      context->varno_to_add_eq = v->varno;
      context->varlevelsup_to_add_eq = v->varlevelsup;
    }
    context->add = true;
    return node;
  }
  if(IsA(node,BoolExpr)){
    /*Deal with OR*/
    BoolExpr * bExpr =(BoolExpr *) node;
      if(bExpr->boolop == OR_EXPR){
        /* elog(LOG,"\nTHERENOT\n"); */
        context->disjunction = true;
        context->varno_to_add_or = -1;
        context->varlevelsup_to_add_or = -1;
        Node * retour =  expression_tree_mutator(node, my_modifier_self, (void *) context);
        context->disjunction =  false;
        context->varno_to_add = -1;
        context->varlevelsup_to_add = -1;
        elog(LOG,"OR check : %s\n",nodeToString(node));
        if(context->add){
          /*Add*/
         context->add = false;
         elog(LOG,"ADD :: %s\n to :: %s\n",nodeToString(node),nodeToString(context->already_constrain_rel));
         context->already_constrain_rel = add_constrain((Node *)context->already_constrain_rel,make_temp_context(node,context->varno_to_add,context->varlevelsup_to_add));
         return NULL;
        }
        return retour;
      }
  }

  if(IsA(node,NullTest)){
    Node * retour = expression_tree_mutator(node, my_modifier_self, (void *) context);
    if(context->add && !context->disjunction){
      context->add = false;
      context->already_constrain_rel = add_constrain((Node *)context->already_constrain_rel,make_temp_context(node,context->varno_to_add,context->varlevelsup_to_add));
      return NULL;
    }
    return retour;

  }

  if(IsA(node,OpExpr)){
    context->equality = true;
    Node * retour = expression_tree_mutator(node, my_modifier_self, (void *) context);
    context->equality = false;
    context->varno_to_add_eq = -1;
    context->varlevelsup_to_add_eq = -1;
    if(context->add && !context->disjunction){
      elog(LOG,"ADD :: %s\n to :: %s\n",nodeToString(node),nodeToString(context->already_constrain_rel));
      context->add = false;
      context->already_constrain_rel = add_constrain((Node *)context->already_constrain_rel,make_temp_context(node,context->varno_to_add,context->varlevelsup_to_add));
      return NULL;
    }
    return retour;
  }
  return expression_tree_mutator(node, my_modifier_self, (void *) context);
}


Node * my_optimize_self (Node *node, context_optimizer_self *context){
}
