\echo 'Double nested';
SELECT o_orderkey
FROM orders
WHERE NOT EXISTS (
  SELECT *
  FROM lineitem
  WHERE l_orderkey = o_orderkey
  AND l_suppkey <> 1
  AND EXISTS (
    SELECT *
    FROM part
    WHERE l_partkey = p_partkey
    AND p_name LIKE '%floral%'));
\echo 'no automatique translation == no splitting'
\echo 'PREFER'
SELECT o_orderkey
FROM orders
WHERE NOT EXISTS (
  SELECT *
  FROM lineitem,part
  WHERE l_orderkey = o_orderkey
  AND l_suppkey <> 1
  AND l_partkey = p_partkey
  AND p_name LIKE '%floral%');

\echo 'From nesting';
SELECT o_orderkey
FROM orders
WHERE NOT EXISTS (
  SELECT *
  FROM (SELECT * FROM lineitem WHERE l_suppkey <> 1) as l1
  WHERE l_orderkey = o_orderkey);
\echo 'doesnt do the work at all'
