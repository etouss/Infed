EXPLAIN ANALYSE SELECT *
FROM nation
WHERE NOT EXISTS(
  SELECT * FROM supplier,((SELECT * FROM orders,lineitem,(SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL) as p WHERE l_partkey = p_partkey AND o_orderkey = l_orderkey)
  UNION ALL
  (SELECT * FROM orders,lineitem,(SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL) as p WHERE l_partkey = 2 AND o_orderkey = l_orderkey)) as olp
  WHERE (s_nationkey = n_nationkey OR s_nationkey = 2)
  AND (s_suppkey = l_suppkey OR l_suppkey = 2)
)
AND n_name = 'FRANCE';
