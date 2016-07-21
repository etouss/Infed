SELECT o_orderkey FROM orders
WHERE NOT EXISTS(SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey = p_partkey
                AND l_suppkey = s_suppkey
                AND s_nationkey = n_nationkey
                AND p_name LIKE '%floral%')
AND NOT EXISTS (SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey = 2
                AND l_suppkey = s_suppkey
                AND s_nationkey = n_nationkey
                AND p_name LIKE '%floral%')
AND NOT EXISTS (SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey = 2
                AND l_suppkey = 2
                AND s_nationkey = n_nationkey
                AND p_name LIKE '%floral%')
AND NOT EXISTS (SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey = 2
                AND l_suppkey = 2
                AND s_nationkey = 2
                AND p_name LIKE '%floral%')
AND NOT EXISTS (SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey = 2
                AND l_suppkey = 2
                AND s_nationkey = 2
                AND p_name = 2)
AND NOT EXISTS(SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey = 2
                AND l_suppkey = s_suppkey
                AND s_nationkey = 2
                AND p_name LIKE '%floral%')
AND NOT EXISTS(SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey = 2
                AND l_suppkey = s_suppkey
                AND s_nationkey = 2
                AND p_name IS NULL)
AND NOT EXISTS(SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey = 2
                AND l_suppkey = s_suppkey
                AND s_nationkey = n_nationkey
                AND p_name  IS NULL)
AND NOT EXISTS(SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey = 2
                AND l_suppkey = = 2
                AND s_nationkey = n_nationkey
                AND p_name IS NULL)
AND NOT EXISTS(SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey = p_partkey
                AND l_suppkey = 2
                AND s_nationkey = n_nationkey
                AND p_name IS NULL)
AND NOT EXISTS(SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey = p_partkey
                AND l_suppkey = 2
                AND s_nationkey = n_nationkey
                AND p_name LIKE '%floral%')
AND NOT EXISTS(SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey = p_partkey
                AND l_suppkey = 2
                AND s_nationkey = 2
                AND p_name LIKE '%floral%')
AND NOT EXISTS(SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey = p_partkey
                AND l_suppkey = 2
                AND s_nationkey = 2
                AND p_name IS NULL)
AND NOT EXISTS(SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey = p_partkey
                AND l_suppkey = s_suppkey
                AND s_nationkey = 2
                AND p_name LIKE '%floral%')
AND NOT EXISTS(SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey = p_partkey
                AND l_suppkey = s_suppkey
                AND s_nationkey = 2
                AND p_name IS NULL)
AND NOT EXISTS(SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey = p_partkey
                AND l_suppkey = s_suppkey
                AND s_nationkey = n_nationkey
                AND p_name IS NULL);





WITH part_view as (SELECT p_partkey FROM part WHERE p_name IS NULL
                  UNION SELECT p_partkey FROM part WHERE p_name LIKE'%floral%'),
     supp_view as(SELECT s_suppkey FROM supplier WHERE s_nationkey = 2
                  UNION
                  SELECT s_suppkey FROM supplier,nation
                  WHERE s_nationkey = n_nationkey AND n_name ='FRANCE')
SELECT o_orderkey FROM orders
WHERE NOT EXISTS (SELECT * FROM lineitem,part_view,supp_view
                  WHERE l_orderkey = o_orderkey
                  AND l_partkey = p_partkey
                  AND l_suppkey = s_suppkey)
AND NOT EXISTS   (SELECT * FROM lineitem,supp_view
                  WHERE l_orderkey = o_orderkey
                  AND l_partkey = 2
                  AND l_suppkey = s_suppkey
                  AND EXISTS (SELECT * FROM part_view))
AND NOT EXISTS   (SELECT * FROM lineitem,part_view
                  WHERE l_orderkey = o_orderkey
                  AND l_partkey = p_partkey
                  AND l_suppkey = 2
                  AND EXISTS (SELECT * FROM supp_view))
AND NOT EXISTS   (SELECT * FROM lineitem
                  WHERE l_orderkey = o_orderkey
                  AND l_partkey = 2
                  AND l_suppkey = 2
                  AND EXISTS (SELECT * FROM part_view)
                  AND EXISTS (SELECT * FROM supp_view));




<WITH lp as ((SELECT * FROM (SELECT * FROM lineitem WHERE l_partkey <> 2) as lineitempositive,
             (SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL ) as partcond
             WHERE l_partkey = p_partkey)
            UNION ALL
             (SELECT * FROM (SELECT * FROM lineitem WHERE l_partkey = 2) as lineitemnull,
             (SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL) as partcond)
           ),
     ls as ((SELECT * FROM (SELECT * FROM lineitem WHERE l_partkey <> 2) as lineitempositive,supplier
             WHERE l_suppkey = s_suppkey)
            UNION ALL
             (SELECT * FROM (SELECT * FROM lineitem WHERE l_partkey = 2) as lineitemnull, supplier)
           ),
     sn as ((SELECT * FROM (SELECT * FROM supplier WHERE s_nationkey <> 2) as supplierpositive,
            (SELECT * FROM nation WHERE n_name = 'FRANCE') as nationcond
             WHERE s_nationkey = n_nationkey)
            UNION ALL
            (SELECT * FROM (SELECT * FROM supplier WHERE s_nationkey = 2) as suppliernull,
            (SELECT * FROM nation WHERE n_name = 'FRANCE') as nationcond)
          ),
     ne as (SELECT lp.l_orderkey FROM lp,ls,sn
           WHERE (lp.l_orderkey,lp.l_suppkey) = (ls.l_orderkey,ls.l_suppkey)
           AND ls.s_suppkey = sn.s_suppkey)
SELECT * FROM ne;

WITH lp as ((SELECT * FROM (SELECT * FROM lineitem WHERE l_partkey IS NOT NULL) as lineitempositive,
             (SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL ) as partcond
             WHERE l_partkey = p_partkey)
            UNION ALL
             (SELECT * FROM (SELECT * FROM lineitem WHERE l_partkey IS NULL) as lineitemnull,
             (SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL) as partcond)
           ),
     ls as ((SELECT * FROM (SELECT * FROM lineitem WHERE l_partkey IS NOT NULL) as lineitempositive,supplier
             WHERE l_suppkey = s_suppkey)
            UNION ALL
             (SELECT * FROM (SELECT * FROM lineitem WHERE l_partkey IS NULL) as lineitemnull, supplier)
           ),
     sn as ((SELECT * FROM (SELECT * FROM supplier WHERE s_nationkey IS NOT NULL) as supplierpositive,
            (SELECT * FROM nation WHERE n_name = 'FRANCE') as nationcond
             WHERE s_nationkey = n_nationkey)
            UNION ALL
            (SELECT * FROM (SELECT * FROM supplier WHERE s_nationkey IS NULL) as suppliernull,
            (SELECT * FROM nation WHERE n_name = 'FRANCE') as nationcond)
          )
  SELECT * FROM lp,ls;







SELECT o_orderkey
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





SELECT o_orderkey
FROM orders
WHERE NOT EXISTS(
  SELECT *
  FROM lineitem,part,supplier,nation
  WHERE l_orderkey = o_orderkey
  AND (l_partkey = p_partkey OR l_partkey IS NULL)
  AND (l_suppkey = s_suppkey OR l_suppkey IS NULL)
  AND (s_nationkey = n_nationkey OR s_nationkey IS NULL)
  AND (p_name LIKE '%'$color'%' OR p_name IS NULL)
  AND n_name = '$nation'
);

SELECT o_orderkey
FROM orders
WHERE NOT EXISTS (SELECT *
                  FROM lineitem
                  WHERE o_orderkey = l_orderkey OR l_orderkey IS NULL
                  AND EXISTS (SELECT *
                                  FROM part
                                  WHERE l_partkey = p_partkey OR l_partkey IS NULL
                                  AND p_name LIKE '%'$color'%' OR p_name IS NULL
                                  )
                  AND EXISTS  (SELECT *
                                  FROM supplier
                                  WHERE l_suppkey = s_suppkey OR l_suppkey IS NULL
                                  AND NOT EXISTS (SELECT *
                                                  FROM nation
                                                  WHERE s_nationkey = n_nationkey OR s_nationkey IS NULL
                                                  AND n_name = '$nation'
                                                  )
                                  )
                  );

SELECT o_orderkey
FROM orders
WHERE NOT EXISTS (SELECT *
                  FROM lineitem
                  WHERE o_orderkey = l_orderkey OR l_orderkey IS NULL
                  AND EXISTS (SELECT *
                              FROM part
                              WHERE l_partkey = p_partkey OR l_partkey IS NULL
                              AND p_name LIKE '%'$color'%' OR p_name IS NULL
                              )
                  AND EXISTS  (SELECT *
                              FROM supplier
                              WHERE l_suppkey = s_suppkey OR l_suppkey IS NULL
                              AND EXISTS (SELECT *
                                          FROM nation
                                          WHERE s_nationkey = n_nationkey
                                          AND n_name = '$nation'
                                          )
                              AND EXISTS (SELECT *
                                          FROM nation
                                          WHERE s_nationkey IS NULL
                                          AND n_name = '$nation'
                                          )
                              )
                  );


SELECT o_orderkey
FROM orders
WHERE NOT EXISTS (SELECT *
                  FROM lineitem
                  WHERE o_orderkey = l_orderkey OR l_orderkey IS NULL
                  AND  EXISTS (SELECT *
                                FROM part
                                WHERE l_partkey = p_partkey OR l_partkey IS NULL
                                AND p_name LIKE '%'$color'%' OR p_name IS NULL
                                )
                  AND  EXISTS  (SELECT *
                                FROM supplier
                                WHERE l_suppkey = s_suppkey
                                AND  EXISTS (SELECT *
                                             FROM nation
                                             WHERE s_nationkey = n_nationkey
                                             AND n_name = '$nation'
                                             )
                                OR  EXISTS (SELECT *
                                             FROM nation
                                             WHERE s_nationkey IS NULL
                                             AND n_name = '$nation'
                                           )d
                                )
                  OR  EXISTS (SELECT *
                               FROM supplier
                               WHERE l_suppkey IS NULL
                               AND  EXISTS (SELECT *
                                            FROM nation
                                            WHERE s_nationkey = n_nationkey
                                            AND n_name = '$nation'
                                            )
                               OR  EXISTS (SELECT *
                                            FROM nation
                                            WHERE s_nationkey IS NULL
                                            AND n_name = '$nation'
                                            )
                               )
                  );
