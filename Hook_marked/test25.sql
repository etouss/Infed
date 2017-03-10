EXPLAIN ANALYSE
SELECT o_orderkey FROM orders
WHERE NOT EXISTS(SELECT * FROM lineitem,(SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL) as part_view,((Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey = n_nationkey)
                UNION ALL (Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey = 2)) as nation_view
                WHERE l_orderkey = o_orderkey
                AND l_partkey = p_partkey
                AND l_suppkey = s_suppkey)
AND NOT EXISTS (SELECT * FROM lineitem,(SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL) as part_view,((Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey = n_nationkey)
                UNION ALL (Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey = 2)) as nation_view
                WHERE l_orderkey = o_orderkey
                AND l_partkey = 2
                AND l_suppkey = s_suppkey)
AND NOT EXISTS (SELECT * FROM lineitem,(SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL) as part_view,((Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey = n_nationkey)
                UNION ALL (Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey = 2)) as nation_view
                WHERE l_orderkey = o_orderkey
                AND l_partkey = 2
                AND l_suppkey = 2)
AND NOT EXISTS(SELECT * FROM lineitem,(SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL) as part_view,((Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey = n_nationkey)
                UNION ALL (Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey = 2))as nation_view
                WHERE l_orderkey = o_orderkey
                AND l_partkey = p_partkey
                AND l_suppkey = 2)
UNION
SELECT o_orderkey FROM orders
WHERE NOT EXISTS(SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL)
UNION
SELECT o_orderkey FROM orders
WHERE NOT EXISTS((Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey = n_nationkey)
                UNION ALL (Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey = 2))
