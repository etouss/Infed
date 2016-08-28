SELECT *
FROM nation
WHERE NOT EXISTS(
  SELECT * FROM supplier,lineitem,part,orders
  WHERE (s_nationkey = n_nationkey OR s_nationkey = 2)
  AND (s_suppkey = l_suppkey OR l_suppkey = 2)
  AND (l_partkey = p_partkey OR l_partkey = 2)
  AND (p_name LIKE '%floral%' OR p_name IS NULL)
  AND (l_orderkey = o_orderkey)
)
AND n_name = 'FRANCE';
