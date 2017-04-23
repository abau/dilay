{- This file is part of Dilay
 - Copyright © 2015-2017 Alexander Bau
 - Use and redistribute under the terms of the GNU General Public License
 -}
import Data.List (nub,sortBy,intercalate)
import Data.Function (on)

type Sigma  = [Bool]
type Vertex = Int
type Edge   = Int
type Face   = Int

adjacentVertices :: Vertex -> [Vertex]
adjacentVertices v = case v of 0 -> [1, 2, 4]
                               1 -> [0, 3, 5]
                               2 -> [0, 3, 6]
                               3 -> [1, 2, 7]
                               4 -> [0, 5, 6]
                               5 -> [1, 4, 7]
                               6 -> [2, 4, 7]
                               7 -> [3, 5, 6]

adjacentEdges :: Vertex -> [Edge]
adjacentEdges v = case v of 0 -> [0,  1,  2]
                            1 -> [0,  4,  5]
                            2 -> [1,  3,  8]
                            3 -> [3,  4, 11]
                            4 -> [2,  6,  7]
                            5 -> [5,  6, 10]
                            6 -> [7,  8,  9]
                            7 -> [9, 10, 11]

edgeToVertices :: Edge -> (Vertex, Vertex)
edgeToVertices e = case e of 0  -> (0,1)
                             1  -> (0,2)
                             2  -> (0,4)
                             3  -> (2,3)
                             4  -> (1,3)
                             5  -> (1,5)
                             6  -> (4,5)
                             7  -> (4,6)
                             8  -> (2,6)
                             9  -> (6,7)
                             10 -> (5,7)
                             11 -> (3,7)

faceToEdges :: Face -> (Edge, Edge, Edge, Edge)
faceToEdges f = case f of 0 -> (0,  5,  6,  2)
                          1 -> (3,  8,  9, 11)
                          2 -> (1,  2,  7,  8)
                          3 -> (4, 11, 10,  5)
                          4 -> (0,  1,  3,  4)
                          5 -> (6, 10,  9,  7)

isSet :: Sigma -> Vertex -> Bool
isSet sigma vertex = sigma !! vertex

isCrossingEdge :: Sigma -> Edge -> Bool
isCrossingEdge sigma edge = ( (isSet sigma v1) && (not $ isSet sigma v2) )
                         || ( (isSet sigma v2) && (not $ isSet sigma v1) )
  where
    (v1,v2) = edgeToVertices edge

isAmbiguousFace :: Sigma -> Face -> Bool
isAmbiguousFace sigma face = all (isCrossingEdge sigma) [e1, e2, e3, e4]
  where
    (e1,e2,e3,e4) = faceToEdges face

isUniformFace :: Sigma -> Face -> Bool
isUniformFace sigma face = not $ any (isCrossingEdge sigma) [e1, e2, e3, e4]
  where
    (e1,e2,e3,e4) = faceToEdges face

countFaces :: Sigma -> (Sigma -> Face -> Bool) -> Int
countFaces sigma f = sum $ map (fromEnum . f sigma) [0..5]

numSet :: Sigma -> Int
numSet = sum . map fromEnum

invertSigma :: Sigma -> Sigma
invertSigma = map not

connectedVertices :: Sigma -> [[Vertex]]
connectedVertices sigma = foldl go [] 
                        $ filter (isSet sigma) [0..7]
  where
    pathTo _       from to | from == to = isSet sigma to 
    pathTo visited from to = any (\a -> pathTo (from:visited) a to) 
                           $ filter (\a -> isSet sigma a && not (a `elem` visited))
                           $ adjacentVertices from

    go css vertex = if css == css'
                    then css ++ [[vertex]]
                    else css'
      where
        css' = do
          cs <- css
          return $ if any (pathTo [] vertex) cs
                   then cs ++ [vertex]
                   else cs

connectedEdges :: Sigma -> [[Edge]]
connectedEdges sigma = map go
                     $ connectedVertices sigma
  where
    go = filter (isCrossingEdge sigma)
       . nub 
       . concatMap (\v -> adjacentEdges v)
    
edgeVertexIndices :: Sigma -> [Int]
edgeVertexIndices sigma = map fst
                        $ sortBy (compare `on` snd)
                        $ concat [ concat $ zipWith (\i -> zip $ repeat i) [0..] es
                                 , map ((,) (-1)) rest ]
  where
    mapSigma = case (numSet sigma, countFaces sigma isAmbiguousFace) of
                 (5, 0) -> id
                 (5, 1) -> invertSigma
                 (5, 3) -> id
                 (5, _) -> undefined
                 (6, 0) -> case countFaces sigma isUniformFace of
                             0 -> invertSigma
                             2 -> id
                             _ -> undefined
                 (6, 1) -> invertSigma
                 (6, _) -> undefined
                 _      -> id

    es     = connectedEdges $ mapSigma sigma
    rest   = filter (\e -> not $ any (elem e) es) [0..11]

printEdgeVertexIndicesTable :: IO ()
printEdgeVertexIndicesTable = putStrLn
                            $ unlines $ do sigma <- sequence $ replicate 8 [False,True]
                                           let is = edgeVertexIndices $ reverse sigma
                                           return $ concat [ "{"
                                                           , intercalate "," $ map show is
                                                           , "}" ]
