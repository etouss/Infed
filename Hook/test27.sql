SELECT count(o_orderkey) FROM orders
WHERE
NOT EXISTS(SELECT * FROM ((SELECT * FROM ((SELECT * FROM lineitem,(SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL) as part_view,((Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey = n_nationkey)
                UNION ALL (Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey IS NULL)) as nation_view
                WHERE l_partkey = p_partkey)
                UNION ALL
                (SELECT * FROM lineitem,(SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL) as part_view,((Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey = n_nationkey)
                              UNION ALL (Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey IS NULL)) as nation_view
                              WHERE l_partkey IS NULL)) as lpsn
                WHERE l_suppkey = s_suppkey)
                UNION ALL
                (SELECT * FROM ((SELECT * FROM lineitem,(SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL) as part_view,((Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey = n_nationkey)
                              UNION ALL (Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey IS NULL)) as nation_view
                              WHERE l_partkey = p_partkey)
                UNION ALL
                (SELECT * FROM lineitem,(SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL) as part_view,((Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey = n_nationkey)
                              UNION ALL (Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey IS NULL)) as nation_view
                              WHERE l_partkey IS NULL)) as lpsn
                WHERE l_suppkey IS NULL)) as lpsn2
WHERE l_orderkey = o_orderkey);
