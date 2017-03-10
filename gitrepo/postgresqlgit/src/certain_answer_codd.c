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
#include "self_optimize.h"
#include "split_optimize.h"
#include "exist_optimize.h"
/* #include "optimize.h" */
/* #include "outfuncs2.h" */
/* #include "pretty_printer.h" */



PG_MODULE_MAGIC;

void _PG_init(void);
void _PG_fini(void);

bool my_walker(Node *node, context_walker_set_constraint *context);
Node * my_mutator (Node *node, context_modifier *context);
static void my_post_parse_analyse_hook(ParseState *pstate, Query *query);

static post_parse_analyze_hook_type prev_post_parse_analyze_hook = NULL;

void
_PG_init(void)
{
  prev_post_parse_analyze_hook = post_parse_analyze_hook;
	post_parse_analyze_hook = my_post_parse_analyse_hook;
}

void _PG_fini(void)
{
  /* elog(LOG,"\n GET IN \n"); */
  post_parse_analyze_hook = prev_post_parse_analyze_hook;
}



bool my_walker(Node *node, context_walker_set_constraint *context){
  if (node == NULL)
 			return false;

  /* elog(LOG,"tag : %d\n",nodeTag(node)); */

  if(IsA(node,BoolExpr)){
    BoolExpr * bExpr =(BoolExpr *) node;
    bool retour = false;
    List * l_true_save = NULL;
    List * l_false_save = NULL;
    List * l_true = NULL;
    List * l_false = NULL;
    int nb_arg = bExpr->args->length;
    switch(bExpr->boolop){
      case NOT_EXPR:
        /*(Unicité des valeur) Setisation sur le true*/
        /*Pre Traitement*/
        l_true_save = context->list_of_not_null_to_be_true;
        l_false_save = context->list_of_not_null_to_be_false;
        context->list_of_not_null_to_be_true = NULL;
        context->list_of_not_null_to_be_false = NULL;

        /*Traitement*/
        retour = expression_tree_walker(node, my_walker, (void *) context);

        /*Post Traitement*/
        /* elog(LOG,"\n List_false_before : %s\n",nodeToString(context->list_of_not_null_to_be_false)); */
        /* elog(LOG,"\n List_true_before : %s\n",nodeToString(context->list_of_not_null_to_be_true)); */
        l_true = context->list_of_not_null_to_be_true;
        l_false = context->list_of_not_null_to_be_false;
        context->list_of_not_null_to_be_true = list_concat(l_true_save,l_false);
        context->list_of_not_null_to_be_false = list_concat(l_false_save,l_true);
        /* elog(LOG,"\n List_false : %s\n",nodeToString(context->list_of_not_null_to_be_false)); */
        /* elog(LOG,"\n List_true : %s\n",nodeToString(context->list_of_not_null_to_be_true)); */

        return retour;

        break;
      case AND_EXPR:
        /*(Unicité des valeur) Setisation sur le true*/
        /*Pre Traitement*/
        l_true_save = context->list_of_not_null_to_be_true;
        l_false_save = context->list_of_not_null_to_be_false;
        context->list_of_not_null_to_be_true = NULL;
        context->list_of_not_null_to_be_false = NULL;

        /*Traitement*/
        retour = expression_tree_walker(node, my_walker, (void *) context);

        /*Post traitement*/
        /* elog(LOG,"\n List_false_before : %s\n",nodeToString(context->list_of_not_null_to_be_false)); */
        /* elog(LOG,"\n List_true_before : %s\n",nodeToString(context->list_of_not_null_to_be_true)); */

        l_true = union_list(context->list_of_not_null_to_be_true);
        l_false = inter_list(context->list_of_not_null_to_be_false,nb_arg);
        context->list_list_true = lappend(context->list_list_true,l_true);
        context->list_list_false = lappend(context->list_list_false,l_false);
        context->list_of_not_null_to_be_true = list_concat(l_true_save,l_true);
        context->list_of_not_null_to_be_false = list_concat(l_false_save,l_false);

        /* elog(LOG,"\n nb_arg : %d\n",nb_arg); */
        /* elog(LOG,"\n List_false : %s\n",nodeToString(context->list_of_not_null_to_be_false)); */
        /* elog(LOG,"\n List_true : %s\n",nodeToString(context->list_of_not_null_to_be_true)); */
        return retour;

        break;
      case OR_EXPR:
        /*(Unicité des valeur) Setisation sur le true*/
        /*Pre Traitement*/
        l_true_save = context->list_of_not_null_to_be_true;
        l_false_save = context->list_of_not_null_to_be_false;
        context->list_of_not_null_to_be_true = NULL;
        context->list_of_not_null_to_be_false = NULL;

        /*Traitement*/
        retour = expression_tree_walker(node, my_walker, (void *) context);

        /*Post traitement*/
        /* elog(LOG,"\n List_false_before : %s\n",nodeToString(context->list_of_not_null_to_be_false)); */
        /* elog(LOG,"\n List_true_before : %s\n",nodeToString(context->list_of_not_null_to_be_true)); */


        l_true = inter_list(context->list_of_not_null_to_be_true,nb_arg);
        l_false = union_list(context->list_of_not_null_to_be_false);
        context->list_list_true = lappend(context->list_list_true,l_true);
        context->list_list_false = lappend(context->list_list_false,l_false);
        context->list_of_not_null_to_be_true = list_concat(l_true_save,l_true);
        context->list_of_not_null_to_be_false = list_concat(l_false_save,l_false);

        /* elog(LOG,"\n nb_arg : %d\n",nb_arg); */
        /* elog(LOG,"\n List_false : %s\n",nodeToString(context->list_of_not_null_to_be_false)); */
        /* elog(LOG,"\n List_true : %s\n",nodeToString(context->list_of_not_null_to_be_true)); */

        return retour;

        break;
    }

  }

  if(context->ready && IsA(node,Var)){
    /* elog(LOG," \n HERE1 \n"); */
    Var * v = getTrueVar(context->current_trueVar,(Var *)node);
    /* elog(LOG," \n HERE2 \n"); */
    if(!isInListTrueVar(context->list_of_not_null_in_op,v)){
      context->list_of_not_null_in_op = lappend(context->list_of_not_null_in_op,v);
    }
    return expression_tree_walker(node, my_walker, (void *) context);
  }

  if(IsA(node,OpExpr)){
     context->ready = true;
     context->list_of_not_null_in_op = NULL;
     bool retour = expression_tree_walker(node, my_walker, (void *) context);
     context->list_of_not_null_to_be_true = list_concat(context->list_of_not_null_to_be_true,context->list_of_not_null_in_op);
     /* elog(LOG,"\n List_op : %s\n",nodeToString(context->list_of_not_null_in_op)); */
     return retour;
  }

  if(IsA(node,Query)){
    Query * q = (Query *) node;
    
    /*Prétraitement*/
    List * save_current_trueVar = context->current_trueVar;
    context->current_trueVar = NULL;
    context->current_trueVar = lappend(context->current_trueVar,save_current_trueVar);
    
    /*Traitement*/
    /*Create trueVars and not_null schema*/
    bool result =  expression_tree_walker((Node *)q->rtable, my_walker, (void *) context);
    /* elog(LOG,"\n TRUE ATT :%s\n",nodeToString(context->trueVars)); */
    context->trueVars = lappend(context->trueVars,context->current_trueVar);
    /* elog(LOG,"\nTRUE ATT :%s \n",nodeToString(context->trueVars)); */
    /*Propagate*/
    result = expression_tree_walker((Node *)q->jointree, my_walker, (void *) context) | result;
    /* elog(LOG,"\n LIST_LIST_TRUE :%s\n",nodeToString(context->list_list_true)); */
    /* elog(LOG,"\n LIST_LIST_FALSE :%s\n",nodeToString(context->list_list_false)); */

    /*Post Traitement*/
    context->current_trueVar = save_current_trueVar;
    return result;
  }
  if(IsA(node,RangeTblEntry)){

    RangeTblEntry * rte = (RangeTblEntry *)node;
    /* create_from_query(rte, NULL,NULL); */
    if(rte->rtekind == 0){
      Oid relid= rte->relid;
      int number_col = get_relnatts(relid);
      AttrNumber i = 0;
      List * relation = NULL;
      for(i=1;i<=number_col;i++){
        /*Create True_var*/
        Var * v = makeNode(Var);
        v->varattno = i;
        /* v->varattno = context->current_varattno; */
        v->vartype = relid;
        relation =  lappend(relation,v);
        /*Add to not_null if needed*/
        if(get_pg_att_not_null(relid,i)){
          context->list_of_not_null_att_schem = lappend(context->list_of_not_null_att_schem,v);
        }
      }
      context->current_trueVar = lappend(context->current_trueVar,relation);
      /* elog(LOG,"\n TEST: %s \n",nodeToString(context->current_trueVar)); */
      return false;
    }
    else if(rte->rtekind == 1){

      Query * subQ = rte->subquery;
      elog(LOG,"Nested Select in FROM not Fully supported yet\n");
      return expression_tree_walker((Node *)subQ->rtable, my_walker, (void *) context);
      
    }
  }
  return expression_tree_walker(node, my_walker, (void *) context);
}



Node * my_mutator (Node *node, context_modifier *context)
  {
    if (node == NULL)
 			return NULL;
    /* elog(LOG,"%d",nodeTag(node)); */
    if(IsA(node,BoolExpr)){
      BoolExpr * bExpr =(BoolExpr *) node;
      if(bExpr->boolop == NOT_EXPR){
        /* elog(LOG,"\nTHERENOT\n"); */
        context->positive = !context->positive;
        Node * retour =  expression_tree_mutator(node, my_mutator, (void *) context);
        context->positive = !context->positive;
        return retour;
      }
      if(bExpr->boolop == OR_EXPR || bExpr->boolop == AND_EXPR){
        /*Pré traitement*/
        List * l_save = context->list_of_not_null_in_current;
        context->list_of_not_null_in_current = list_copy(l_save);
        if(context->positive){
          context->list_of_not_null_in_current = list_concat(context->list_of_not_null_in_current,list_nth(context->list_list_true,context->list_list_true->length - context->where_i_am - 1));
        }
        else{
          context->list_of_not_null_in_current = list_concat(context->list_of_not_null_in_current,list_nth(context->list_list_false,context->list_list_true->length - context->where_i_am - 1));
        }
        /* elog(LOG,"\n LIST_NOT_NULL: %s \n",nodeToString(context->list_of_not_null_in_current)); */
        context->where_i_am ++;
        Node * retour =  expression_tree_mutator(node, my_mutator, (void *) context);
        /*Post traitement*/
        context->list_of_not_null_in_current = l_save;

        return retour;
      }
    }
    if(IsA(node,Query)){
      Query * q = (Query *) node;
      /* context->current_varlevelsup ++; */
      List * save_current_trueVar = context->current_trueVar;
      context->current_trueVar = list_nth(context->trueVars,context->where_i_am_querry);
      context->where_i_am_querry = context->where_i_am_querry + 1;
      q->jointree = (FromExpr *)expression_tree_mutator((Node *) q->jointree, my_mutator, (void *) context);
      /* context->where_i_am_querry = context->where_i_am_querry - 1; */
      context->current_trueVar = save_current_trueVar;
      /* context->current_varlevelsup --; */

      /*Deal with Self*/
      /* context_optimizer_self * ctx_self = make_context_optimizer_self(); */
      /* q->jointree = my_modifier_self((Node *)q->jointree,ctx_self); */
      /* #<{(| elog(LOG,"HERE 2 :: %s \n",nodeToString(ctx_self->already_constrain_rel)); |)}># */
      /* rewrite_self(q,ctx_self->already_constrain_rel); */
      /*  */
      /* #<{(|Deal with split|)}># */
      /* if(!context->positive){ */
      /*   List * args = trueSplit((Node *)q->jointree->quals); */
      /*   List * result = NULL; */
      /*   const ListCell *cell; */
  	  /*   foreach(cell, args) */
	    /*   { */
      /*     Node * t = lfirst(cell); */
      /*     Query * nq = copyObject(q); */
      /*     nq->jointree->quals = t; */
      /*     context_optimizer_self * ctx_self = make_context_optimizer_self(); */
      /*     nq->jointree = my_modifier_self((Node *)nq->jointree,ctx_self); */
      /*     #<{(| elog(LOG,"HERE 2 :: %s \n",nodeToString(ctx_self->already_constrain_rel)); |)}># */
      /*     rewrite_self(nq,ctx_self->already_constrain_rel); */
      /*     temp_context_exists * ctx_e = make_temp_context_exists(nq->rtable->length); */
      /*     create_connexe((Node *)nq->jointree,ctx_e); */
      /*     elog(LOG,"NQ::: %s\n",nodeToString(ctx_e->composante_connexe_sub_level)); */
      /*     elog(LOG,"NQ2::: %s\n",nodeToString(ctx_e->list_composantes_connexes)); */
      /*      */
      /*     List * list = NULL; */
      /*     list = ctx_e->list_composantes_connexes; */
      /*     elog(LOG,"HERHEHEHRHEHHER P \n"); */
      /*     while(list != NULL && list->length != 0){ */
      /*       elog(LOG,"HERHEHEHRHEHHER 0%s \n",nodeToString(list_nth(list,0))); */
      /*       rewrite_connexe(nq,list_nth(list,0)); */
      /*       List * tmp = NULL; */
      /*       elog(LOG,"HERHEHEHRHEHHER 1\n"); */
      /*       if(list->length < 1) */
      /*         break; */
      /*       for(int i = 1;i<list->length;i++){ */
      /*         List * tm2 = NULL; */
      /*          */
      /*         const ListCell *cell; */
      /*         foreach(cell, list_nth(list,i)) */
      /*        	{ */
      /*           elog(LOG,"HERHEHEHRHEHHER 3\n"); */
      /*           tm2 = lappend_int(tm2,deacrease(lfirst_int(cell),list_nth(list,0))); */
      /*         } */
      /*         elog(LOG,"HERHEHEHRHEHHER 4\n"); */
      /*         tmp = lappend(tmp,tm2); */
      /*       } */
      /*       list = copyObject(tmp); */
      /*      } */
      /*     elog(LOG,"HERHEHEHRHEHHER 2\n"); */
      /*     elog(LOG,"QUUUQUUUQUUU ::: %s \n",nodeToString(nq->jointree)); */
      /*   }   */
      /* } */
      return node;
    }
    if(IsA(node,OpExpr)){
      /* elog(LOG,"\nTHEREOP = \n"); */
      OpExpr * oExpr = (OpExpr *) node;
      if(!context->positive && oExpr->opno == 518){ /*<>*/
        context->ready = true;
      }
      if(!context->positive && oExpr->opno == 416){ /* = */
        /* elog(LOG,"\nTHERENOT = \n"); */
        context->ready = true;
      }
      if(!context->positive && oExpr->opno == 98){ /* = */
        /* elog(LOG,"\nTHERENOT = \n"); */
        context->ready = true;
      }
      if(!context->positive && oExpr->opno == 521){ /* > */
        /* elog(LOG,"\nTHERENOT = \n"); */
        context->ready = true;
      }
      if(!context->positive && oExpr->opno == 525){ /* >= */
        /* elog(LOG,"\nTHERENOT = \n"); */
        context->ready = true;
      }
      if(!context->positive && oExpr->opno == 523){ /* <= */
        /* elog(LOG,"\nTHERENOT = \n"); */
        context->ready = true;
      }
      if(!context->positive && oExpr->opno == 97){ /* < */
        /* elog(LOG,"\nTHERENOT = \n"); */
        context->ready = true;
      }
      if(!context->positive && oExpr->opno == 1209){ /* LIKE */
        /* elog(LOG,"\nTHERENOT = \n"); */
        context->ready = true;
      }


      Node * result = expression_tree_mutator(node, my_mutator, (void *) context);
      if(context->constraint_to_add != NULL){
        if(!context->positive /* && oExpr->opno == 518 */){
          context->constraint_to_add = lappend(context->constraint_to_add,result);
          Expr * to_return = makeBoolExpr(OR_EXPR,context->constraint_to_add,-1);
          context->constraint_to_add = NULL;
          context->ready = false;
          /* elog(LOG,"inter :%s\n",nodeToString(to_return)); */
          return (Node *)to_return;
        }
        /* if(!context->positive && oExpr->opno == 96){ */
        /*   context->constraint_to_add = lappend(context->constraint_to_add,result); */
        /*   Node * to_return = makeBoolExpr(OR_EXPR,context->constraint_to_add,-1); */
        /*   context->constraint_to_add = NULL; */
        /*   context->ready = false; */
        /*   return to_return; */
        /* } */
      }
      else
        return result;

    }
 		if(context->ready && IsA(node, Var))
 		{
      /* elog(LOG,"\n LIST_NOT_NULL: %s \n",nodeToString(context->list_of_not_null_in_current)); */
      Var * v = (Var *) node;
      Var * rv = getTrueVar(context->current_trueVar,v);
      /* elog(LOG,"\nTRUE VARS : %s \n",nodeToString(context->current_trueVar)); */
      /* elog(LOG,"\nV check %s \n",nodeToString(v)); */
      /* elog(LOG,"\nRV check %s \n",nodeToString(rv)); */
      /* elog(LOG,"\nNOT NULL CUR: %s \n",nodeToString(context->list_of_not_null_in_current)); */
      if(!isInListTrueVar(context->list_of_not_null_in_current,rv)){
        /* elog(LOG,"\nHERE !\n"); */
        v->location = -1;
        NullTest * null_check =  makeNode(NullTest);
        null_check->nulltesttype = IS_NULL;
        /* null_check->nulltesttype = context->positive?IS_NOT_NULL:IS_NULL; */
        null_check->arg = (Expr *)node;
        null_check->location = -1;
        context->constraint_to_add = lappend(context->constraint_to_add,null_check);
      }
 		}
 		return expression_tree_mutator(node, my_mutator, (void *) context);
  }



static void
my_post_parse_analyse_hook(ParseState *pstate, Query *query)
{
  /* struct rusage * r_usage = malloc(sizeof(struct rusage)); */
  /* getrusage(0, r_usage); */
  /* elog(LOG,"DATA : %ld; %ld; %ld",r_usage->ru_ixrss,r_usage->ru_idrss,r_usage->ru_isrss); */
  /* debug_pretty_print(true); */
  /* debug_print_plan(true); */
  /* elog(LOG,"%s\n",nodeToString((Node *)query->jointree)); */
  context_walker_set_constraint * ctx = make_context_walker_set_constraint();
  my_walker((Node *)query,ctx);
  /* expression_tree_walker((Node *)query->rtable,my_walker,ctx); */
  /* expression_tree_walker((Node *)query->rtable,my_walker,ctx); */
  /* free_ctx_walker(ctx); */
  /* elog(LOG,"\n TrueVars :%s\n",nodeToString(ctx->trueVars)); */
  /* elog(LOG,"\nKind %d\n",query->type); */
  /* query->jointree = expression_tree_mutator((Node *)query->jointree,my_mutator,make_context_modifier(true)); */
  /* elog(LOG,"AFTER \n"); */
  /* elog(LOG,"\nNOT NULL SCHEMA: %s \n",nodeToString(ctx->list_of_not_null_att_schem)); */
  /* elog(LOG,"HERE 0\n"); */
  context_modifier * ctx_m = make_context_modifier(true,ctx);
  my_mutator((Node *)query,ctx_m);
  /* elog(LOG,"HERE 1\n"); */
  /* elog(LOG,"%s\n",nodeToString(query));   */
  /* context_optimizer_self * ctx_self = make_context_optimizer_self(); */
  /* elog(LOG,"HERE 2\n"); */
  /* my_modifier_self((Node *)query,ctx_self); */

  /* rewrite_self(query,ctx_self->already_constrain_rel); */
  /* elog(LOG,"%s\n",nodeToString(query)); */
  /* elog(LOG,"HERE 3\n"); */
  /* elog(LOG,"ALREADY CONSTRAINT : %s\n",nodeToString(ctx_self->already_constrain_rel)); */
  /* *query = *nq; */

  optimize_self_constraint_deep(query);
  elog(LOG,"SELF :: %s \n",nodeToString(query->jointree->quals));
  optimize_split_deep(query);
  elog(LOG,"SPLIT :: %s \n",nodeToString(query->jointree->quals));
  optimize_exist_deep(query);
  /* elog(LOG,"EXIS:: %s \n",nodeToString(query->jointree->quals)); */


  


  
  /* post_parse_analyze_hook = prev_post_parse_analyze_hook; */
  /* post_parse_analyze_hook = prev_post_parse_analyze_hook; */

  /* elog(LOG,"%s\n",nodeToString(query)); */
  /* free_context_modifier(ctx_m); */
  /* free_context_walker_set_constraint(ctx); */
  /* getrusage(0, r_usage); */
  /* elog(LOG,"DATA : %ld; %ld; %ld",r_usage->ru_ixrss,r_usage->ru_idrss,r_usage->ru_isrss); */
  /* _PG_fini(); */
}
