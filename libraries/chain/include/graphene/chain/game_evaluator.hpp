/*
 * Copyright (c) 2015 Cryptonomex, Inc., and contributors.
 *
 * The MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#pragma once
#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/game_object.hpp>

namespace graphene { namespace chain {

class game_create_evaluator : public evaluator<game_create_evaluator>
{
public:
   typedef game_create_operation operation_type;

   void_result do_evaluate( const game_create_operation& o );
   object_id_type do_apply( const game_create_operation& o ) ;
};

class game_update_evaluator : public evaluator<game_update_evaluator>
{
public:
   typedef game_update_operation operation_type;

   void_result do_evaluate( const game_update_operation& o );
   void_result do_apply( const game_update_operation& o );

   const game_object* game_to_update;
};


class game_play_evaluator : public evaluator<game_play_evaluator>
{
public:
   typedef game_play_operation operation_type;

   void_result do_evaluate( const game_play_operation& o);
   void_result do_apply( const game_play_operation& o);

   const game_object* game_to_play;
};
    
    class game_buy_chips_evaluator : public evaluator<game_buy_chips_evaluator>
    {
    public:
        typedef game_buy_chips_operation operation_type;
        
        void_result do_evaluate( const game_buy_chips_operation& o);
        void_result do_apply( const game_buy_chips_operation& o);
        
        const asset_dynamic_data_object* _asset_dyn_data = nullptr;
    };

} } // graphene::chain
