SELECT count(o_orderkey)
FROM orders
WHERE NOT EXISTS(
  SELECT *
  FROM lineitem,part,supplier,nation
  WHERE l_orderkey = o_orderkey
  AND (l_partkey = p_partkey)
  AND (l_suppkey = s_suppkey)
  AND (s_nationkey = n_nationkey)
  AND (p_name LIKE '%floral%')
  AND n_name = 'FRANCE'
);

SELECT count(o_orderkey) FROM orders
WHERE NOT EXISTS(SELECT * FROM lineitem,(SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL) as part_view,supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as nation_view
                WHERE l_orderkey = o_orderkey
                AND l_partkey = p_partkey
                AND l_suppkey = s_suppkey
                AND s_nationkey = n_nationkey)
AND NOT EXISTS (SELECT * FROM lineitem,supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as nation_view
                WHERE l_orderkey = o_orderkey
                AND l_partkey IS NULL
                AND l_suppkey = s_suppkey
                AND s_nationkey = n_nationkey
                AND EXISTS (SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL))
AND NOT EXISTS (SELECT * FROM lineitem
                WHERE l_orderkey = o_orderkey
                AND l_partkey IS NULL
                AND l_suppkey IS NULL
                AND EXISTS (SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL)
                AND EXISTS (SELECT * FROM supplier WHERE s_nationkey IS NULL)
                AND EXISTS (SELECT * FROM nation WHERE n_name = 'FRANCE'))
AND NOT EXISTS (SELECT * FROM lineitem
                WHERE l_orderkey = o_orderkey
                AND l_partkey IS NULL
                AND l_suppkey IS NULL
                AND EXISTS (SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL)
                AND EXISTS  (SELECT * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as nation_view WHERE s_nationkey = n_nationkey))
AND NOT EXISTS(SELECT * FROM lineitem,supplier
                WHERE l_orderkey = o_orderkey
                AND l_partkey IS NULL
                AND l_suppkey = s_suppkey
                AND s_nationkey IS NULL
                AND EXISTS (SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL)
                AND EXISTS (SELECT * FROM nation WHERE n_name = 'FRANCE'))
AND NOT EXISTS(SELECT * FROM lineitem,(SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL) as part_view
                WHERE l_orderkey = o_orderkey
                AND l_partkey = p_partkey
                AND l_suppkey IS NULL
                AND EXISTS (SELECT * FROM supplier WHERE s_nationkey IS NULL)
                AND EXISTS (SELECT * FROM nation WHERE n_name = 'FRANCE'))
AND NOT EXISTS(SELECT * FROM lineitem,(SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL) as part_view
                WHERE l_orderkey = o_orderkey
                AND l_partkey = p_partkey
                AND l_suppkey IS NULL
                AND EXISTS  (SELECT * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as nation_view WHERE s_nationkey = n_nationkey))
AND NOT EXISTS(SELECT * FROM lineitem,(SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL) as part_view,supplier
                WHERE l_orderkey = o_orderkey
                AND l_partkey = p_partkey
                AND l_suppkey = s_suppkey
                AND s_nationkey IS NULL
                AND EXISTS (SELECT * FROM nation WHERE n_name = 'FRANCE'));

SELECT count(o_orderkey)
FROM orders
WHERE NOT EXISTS(SELECT * FROM lineitem,(SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL) as part_view,((Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey = n_nationkey)
                UNION ALL (Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey IS NULL)) as nation_view
                WHERE l_orderkey = o_orderkey
                AND l_partkey = p_partkey
                AND l_suppkey = s_suppkey)
AND NOT EXISTS (SELECT * FROM lineitem,((Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey = n_nationkey)
                UNION ALL (Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey IS NULL)) as nation_view
                WHERE l_orderkey = o_orderkey
                AND l_partkey IS NULL
                AND l_suppkey = s_suppkey
                AND EXISTS(SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL))
AND NOT EXISTS (SELECT * FROM lineitem
                WHERE l_orderkey = o_orderkey
                AND l_partkey IS NULL
                AND l_suppkey IS NULL
                AND EXISTS(SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL)
                AND EXISTS((Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey = n_nationkey)
                                UNION ALL (Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey IS NULL)))
AND NOT EXISTS(SELECT * FROM lineitem,(SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL) as part_view
                WHERE l_orderkey = o_orderkey
                AND l_partkey = p_partkey
                AND l_suppkey IS NULL
                AND EXISTS((Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey = n_nationkey)
                                UNION ALL (Select * FROM supplier,(SELECT * FROM nation WHERE n_name = 'FRANCE') as n WHERE s_nationkey IS NULL)))
