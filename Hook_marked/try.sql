EXPLAIN ANALYSE SELECT *
FROM nation
WHERE NOT EXISTS(
  SELECT * FROM supplier,lineitem,part,orders
  WHERE (s_nationkey = n_nationkey)
  AND (s_suppkey = l_suppkey)
  AND (l_partkey = p_partkey)
  AND (p_name LIKE '%floral%')
  AND (l_orderkey = o_orderkey)
)
AND n_name = 'FRANCE';
