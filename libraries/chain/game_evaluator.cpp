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

#include <fc/smart_ref_impl.hpp>

#include <graphene/chain/game_evaluator.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/chain/exceptions.hpp>
#include <graphene/chain/hardfork.hpp>
#include <graphene/chain/internal_exceptions.hpp>
#include <graphene/chain/special_authority.hpp>
#include <graphene/chain/special_authority_object.hpp>
#include <graphene/chain/is_authorized_asset.hpp>

#include <algorithm>

namespace graphene { namespace chain {


void_result game_create_evaluator::do_evaluate( const game_create_operation& op )
{ try {
   database& d = db();

   FC_ASSERT( d.find_object(op.issuer), "Invalid owner account specified." );

   auto& game_indx = d.get_index_type<game_index>();
   if( op.name.size() )
   {
      auto current_game_itr = game_indx.indices().get<by_game_name>().find( op.name );
      FC_ASSERT( current_game_itr == game_indx.indices().get<by_game_name>().end() );
   }

   return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

object_id_type game_create_evaluator::do_apply( const game_create_operation& o )
{ try {

   database& d = db();

   const auto& new_game_object = d.create<game_object>( [&]( game_object& obj ){
         obj.name = o.name;
         obj.description = o.description;
         obj.issuer = o.issuer;
         obj.script_code = o.script_code;
   });

   return new_game_object.id;
} FC_CAPTURE_AND_RETHROW((o)) }


void_result game_update_evaluator::do_evaluate( const game_update_operation& o )
{ try {
   database& d = db();

   const game_object& g = o.game_to_update(d);
   auto g_copy = g;
   g_copy.description = o.new_description;
   g_copy.script_code = o.new_script_code;
   // g_copy.validate();

   if( o.new_issuer )
   {
     FC_ASSERT(d.find_object(*o.new_issuer));
   }

   game_to_update = &g;
   FC_ASSERT( o.issuer == g.issuer, "", ("o.issuer", o.issuer)("g.issuer", g.issuer) );

   return void_result();
} FC_CAPTURE_AND_RETHROW( (o) ) }

void_result game_update_evaluator::do_apply( const game_update_operation& o )
{ try {
   database& d = db();

   d.modify(*game_to_update, [&](game_object& g) {
      if( o.new_issuer )
         g.issuer = *o.new_issuer;
      g.description = o.new_description;
      g.script_code = o.new_script_code;
   });

   return void_result();
} FC_CAPTURE_AND_RETHROW( (o) ) }

void_result game_play_evaluator::do_evaluate(const game_play_evaluator::operation_type& o)
{ try {
   database& d = db();

   game_to_play = &d.get(o.game_to_play);
   
   FC_ASSERT( d.find_object(o.player), "Invalid player account specified." );
   
   d.on_game_evaluate( o );

   // TODO: bts::game::client::get_current().get_v8_engine( ogame->name )->evaluate( eval_state, input.game_id, input.data );

   return void_result();
} FC_CAPTURE_AND_RETHROW( (o) ) }

void_result game_play_evaluator::do_apply(const game_play_evaluator::operation_type& o)
{ try {
   database& d = db();

   const auto& new_game_play_object = d.create<game_play_object>( [&]( game_play_object& obj ){
         obj.game_id = o.game_to_play;
         obj.input = o.input_data;
         obj.ref_block_num = trx_state->_trx->ref_block_num;
         obj.ref_trx_id = trx_state->_trx->id();
   });
   
   d.on_game_apply( o, new_game_play_object );
   // TODO: bts::game::client::get_current().get_v8_engine( ogame->name )->play( eval_state, input.game_id, input.data );

   return void_result();
} FC_CAPTURE_AND_RETHROW( (o) ) }
    
    
    
    
    void_result game_buy_chips_evaluator::do_evaluate(const game_buy_chips_evaluator::operation_type& op)
    { try {
        
        database& d = db();
        
        const account_id_type& buyer        = op.buyer;
        const asset_object& sell_asset_obj    = op.amount_to_sell.asset_id(d);
        const asset_object& receive_asset_obj = op.amount_to_receive.asset_id(d);
 
        FC_ASSERT( is_authorized_asset( d, d.get(buyer), sell_asset_obj ) );
        FC_ASSERT( is_authorized_asset( d, d.get(buyer), receive_asset_obj ) );
        
        FC_ASSERT( d.get_balance( d.get(buyer), sell_asset_obj ) >= op.amount_to_sell, "insufficient balance",
                ("balance",d.get_balance(d.get(buyer), sell_asset_obj))("amount_to_sell",op.amount_to_sell) );
        
        
        _asset_dyn_data = &receive_asset_obj.dynamic_asset_data_id(d);
        
        FC_ASSERT( (_asset_dyn_data->current_supply + op.amount_to_receive.amount) <= receive_asset_obj.options.max_supply );

   
        return void_result();
    } FC_CAPTURE_AND_RETHROW( (op) ) }
    
    void_result game_buy_chips_evaluator::do_apply(const game_buy_chips_evaluator::operation_type& op)
    { try {
        
        database& d = db();
        FC_ASSERT( d.find_object(op.buyer), "Invalid owner account specified." );
        
        db().adjust_balance(op.buyer, -op.amount_to_sell);
        db().adjust_balance(op.buyer, op.amount_to_receive);
        
        db().modify( *_asset_dyn_data, [&]( asset_dynamic_data_object& data ){
            data.current_supply += op.amount_to_receive.amount;
            data.current_collateral += op.amount_to_sell.amount;
        });
  
        return void_result();
    } FC_CAPTURE_AND_RETHROW( (op) ) }
    

} } // graphene::chain
