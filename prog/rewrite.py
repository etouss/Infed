import sqlparse
from enum import Enum

#Test Query
#queries = 'Select attribut1,attribut2 FROM Table1,Table2 WHERE attribut1 = 2 and attribut1 = attribut2 AND NOT EXISTS (SELECT * FROM Table3 WHERE attribut3 = 4);'
queries = 'SELECT a FROM T WHERE NOT EXISTS (SELECT * FROM R) AND c = 2 AND b NOT NULL;'

#Create a syntaxic tree for the SQL query.

class QueryType(Enum):
    posible = 1
    certain = 2

def deep_printer(token):
        if(token.is_whitespace()):
            return
        elif(token.is_group()):
            print("Group : "+token.__class__.__name__)
            idx = -1
            token_in = token.token_next(idx,False)
            while(token_in != None):
                deep_printer(token_in)
                idx += 1
                token_in = token.token_next(idx,False)
        else:
            print(token)

def rewritting_certain(token):
    if(token.is_whitespace()):
        return
    elif(token.is_group()):
        return


def create_ast(q):
    parsed_queries = sqlparse.parse(q)
    for parsed_query in parsed_queries :
        deep_printer(parsed_query)

        #print(parsed_query.get_type())
        #idx = -1
        #token = parsed_query.token_next(idx)
        #while(token != None):
        #    print(token)
        #    if(token.is_group()):
        #        print(token.tokens)
        #    idx = idx +1
        #    token = parsed_query.token_next(idx)


#create_ast(queries)
print(sqlparse.format(queries,reindent=True))
