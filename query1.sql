SELECT s_suppkey, o_orderkey
FROM supplier, lineitem l1, orders, nation
WHERE s_suppkey = l1.l_suppkey
AND o_orderkey = l1.l_orderkey
AND o_orderstatus = 'F'
AND l1.l_receiptdate > l1.l_commitdate
AND s_nationkey = n_nationkey
AND n_name = 'FRANCE'
AND EXISTS (
  SELECT *
  FROM lineitem l2
  WHERE l2.l_orderkey = l1.l_orderkey
  AND l2.l_suppkey <> l1.l_suppkey )
AND NOT EXISTS (
  SELECT *
  FROM lineitem l3
  WHERE l3.l_orderkey = l1.l_orderkey
  AND l3.l_suppkey <> l1.l_suppkey
  AND l3.l_receiptdate > l3.l_commitdate );


SELECT s_suppkey, o_orderkey
FROM supplier, lineitem l1, orders, nation
WHERE s_suppkey = l1.l_suppkey
AND o_orderkey = l1.l_orderkey
AND o_orderstatus = 'F'
AND l1.l_receiptdate > l1.l_commitdate
AND s_nationkey = n_nationkey
AND n_name = 'FRANCE'
AND EXISTS (
  SELECT *
  FROM lineitem l2
  WHERE l2.l_orderkey = l1.l_orderkey
  AND l2.l_suppkey <> l1.l_suppkey )
AND NOT EXISTS (
  SELECT *
  FROM lineitem l3
  WHERE l3.l_orderkey = l1.l_orderkey
  AND ( l3.l_suppkey <> l1.l_suppkey OR l3.l_suppkey IS NULL )
  AND ( l3.l_receiptdate > l3.l_commitdat OR l3.l_receiptdate IS NULL OR l3.l_commitdate IS NULL ))
<<<<<<< HEAD





SELECT ord_id FROM Orders 
WHERE NOT EXISTS  
 (SELECT * FROM Payments 
	WHERE Payments.ord_id = Orders.ord_id)

SELECT ord_id FROM Orders 
WHERE NOT EXISTS  
 (SELECT * FROM Payments 
	WHERE Payments.ord_id = Orders.ord_id
  0R Payments.ord_id IS NULL)
=======
>>>>>>> 79d6f8b0e81303ff3b2d926824ca6387169e9d42
