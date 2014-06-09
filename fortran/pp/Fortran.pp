[
   Comment                       -- _1,
   None                          -- ,
   Label                         -- _1,
   REAL                          -- KW["REAL ::"],
   INTEGER                       -- KW["INTEGER ::"],

   #R201 --,
   OfpProgram                    -- _1 _2,
   OfpProgram.1:opt              -- ,

   #R204 --,
   OfpSpecPart                   -- H hs=0[_1],

   #R208 --,
   OfpExecPart                   -- H hs=0[_1],
   OfpFuncPart                   -- H hs=0[_1],

   #R303 --,
   OfpName                       -- H hs=1[_1],
   OfpEndProgramStmt             -- H hs=1[_1 "END PROGRAM" _2],

   #R404 --,
   OfpType                       -- H hs=1[_1 _2],

   #R407 --,
   OfpLiteral                    -- H hs=1[_1 _2],

   #R501 --,
   OfpTypeDeclarationStmt        -- H hs=1[H hs=3[_1] _2 _3 _4],

   #R502 --,
   Some                          -- _1,
   Some                          -- _1 _2,
   
   #R503 --,
   OfpEntityDecl                 -- _1 _2 _3 _4 _5,

   #R602 --,
   OfpVarDef                     -- _1,

   #R732 --,
   OfpAssignmentStmt             -- H hs=1[H hs=3[_1] _2 KW["="] _3],
   OfpAssignmentStmt.2:opt       -- ,

   #R1101 --,
   OfpMainProgram                -- V[H[_1] _2],
   OfpMainProgram.1:opt          -- ,

   #R1102 --,
   OfpProgramStmt                --  _1 KW["PROGRAM"] _2,
   
   #R1205 --,
   OfpScope                      -- V vs=1 is=0[H hs=1[_4] _1 _2 _3 _5 _6],


   OfpPrefix                           -- _1,
   OfpPrefix.1:iter-star               -- _1,

   ELEMENTAL                           -- "ELEMENTAL",
   IMPURE                              -- "IMPURE",
   MODULE                              -- "MODULE",
   PURE                                -- "PURE",
   RECURSIVE                           -- "RECURSIVE",

   OfpLanguageBinding                  -- "BIND(C" _1 ")",
   OfpResultName                       -- "RESULT" _1,
   OfpSuffix                           -- _1,
   OfpSuffix.1:iter-star               -- _1,

   #R1233 --,
   OfpSubroutine                       -- _2 "!" _1,
   OfpSubroutineStmt                   -- _1 _4 "SUBROUTINE" _2 "(" _3 ")" _5,
   OfpEndSubroutineStmt                -- H hs=1[_1 "END SUBROUTINE" _2],

   STAR                                -- "*",
   OfpArgList                          -- _1,
   OfpArgList.1:iter-star-sep          -- _1 ","

]