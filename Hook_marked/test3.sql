WITH part_view as (SELECT p_partkey FROM part WHERE p_name IS NULL
                                  UNION SELECT p_partkey FROM part WHERE p_name LIKE'%floral%'),
                      supp_view as(SELECT s_suppkey FROM supplier WHERE s_nationkey IS NULL
                                  UNION
                                  SELECT s_suppkey FROM supplier,nation
                                  WHERE s_nationkey = n_nationkey AND n_name ='FRANCE')
SELECT count(o_orderkey) FROM orders
WHERE NOT EXISTS (SELECT * FROM lineitem,part_view,supp_view
                  WHERE l_orderkey = o_orderkey
                  AND l_partkey = p_partkey
                  AND l_suppkey = s_suppkey)
AND NOT EXISTS   (SELECT * FROM lineitem,supp_view
                  WHERE l_orderkey = o_orderkey
                  AND l_partkey IS NULL
                  AND l_suppkey = s_suppkey
                  AND EXISTS (SELECT * FROM part_view))
AND NOT EXISTS   (SELECT * FROM lineitem,part_view
                  WHERE l_orderkey = o_orderkey
                  AND l_partkey = p_partkey
                  AND l_suppkey IS NULL
                  AND EXISTS (SELECT * FROM supp_view))
AND NOT EXISTS   (SELECT * FROM lineitem
                  WHERE l_orderkey = o_orderkey
                  AND l_partkey IS NULL
                  AND l_suppkey IS NULL
                  AND EXISTS (SELECT * FROM part_view)
                  AND EXISTS (SELECT * FROM supp_view));
