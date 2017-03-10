SELECT count((c_custkey, c_nationkey))
FROM customer
WHERE c_nationkey = 1
AND c_acctbal > (
  SELECT avg(c_acctbal)
  FROM customer
  WHERE c_acctbal > 0.00
  AND c_nationkey = 1 )
AND NOT EXISTS (
  SELECT *
  FROM orders
  WHERE o_custkey = c_custkey );

SELECT count((c_custkey, c_nationkey))
FROM customer
WHERE c_nationkey = 1
AND c_acctbal > (
  SELECT AVG(c_acctbal)
  FROM customer
  WHERE c_acctbal > 0.00
  AND c_nationkey = 1 )
AND NOT EXISTS (
  SELECT *
  FROM orders
  WHERE o_custkey = c_custkey )
AND NOT EXISTS (
  SELECT *
  FROM orders
  WHERE o_custkey IS NULL );
