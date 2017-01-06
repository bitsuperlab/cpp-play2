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
#include <graphene/chain/protocol/base.hpp>
#include <graphene/chain/protocol/ext.hpp>
#include <graphene/chain/protocol/special_authority.hpp>
#include <graphene/chain/protocol/types.hpp>

namespace graphene { namespace chain {

    /**
     *  @ingroup operations
     */
    struct game_create_operation : public base_operation
    {
      struct fee_parameters_type
      {
         uint64_t basic_fee      = 500*GRAPHENE_BLOCKCHAIN_PRECISION; ///< the cost to register the cheapest non-free game
         uint64_t premium_fee    = 20000*GRAPHENE_BLOCKCHAIN_PRECISION; ///< the cost to register the cheapest non-free game
         uint32_t price_per_kbyte = GRAPHENE_BLOCKCHAIN_PRECISION;
      };

      asset            fee;

      ///Names are a more complete description and may contain any kind of characters or spaces.
      std::string      name;

      /// Describes the game and its purpose.
      std::string      description;

      /// Game only be issued by individuals that have be registered.
      account_id_type  issuer;

      /// The url of the game's rule script
      std::string      script_code;

      account_id_type fee_payer()const { return issuer; }
      void            validate()const;
      share_type      calculate_fee(const fee_parameters_type& )const;

      void get_required_active_authorities( flat_set<account_id_type>& a )const
      {
         // registrar should be required anyway as it is the fee_payer(), but we insert it here just to be sure
         a.insert( issuer );
      }
    };

    struct game_update_operation : public base_operation
    {
      struct fee_parameters_type
      {
         uint64_t fee      = 20*GRAPHENE_BLOCKCHAIN_PRECISION;
         uint32_t price_per_kbyte = GRAPHENE_BLOCKCHAIN_PRECISION;
      };

      asset            fee;

      account_id_type  issuer;

      game_id_type     game_to_update;

      /// Describes the asset and its purpose.
      std::string      new_description;

      /// The url of the game's rule script
      std::string      new_script_code;

      /// If the asset is to be given a new issuer, specify his ID here.
      optional<account_id_type>   new_issuer;

      extensions_type             extensions;

      account_id_type fee_payer()const { return issuer; }
      void            validate()const;
      share_type      calculate_fee(const fee_parameters_type& )const;
    };

    struct game_play_operation : public base_operation
    {
      struct fee_parameters_type
      {
        share_type fee             = 20 * GRAPHENE_BLOCKCHAIN_PRECISION;
        uint32_t   price_per_kbyte = GRAPHENE_BLOCKCHAIN_PRECISION;
      };

      asset            fee;

      account_id_type  player;

      game_id_type     game_to_play;

      fc::variant      input_data;

      account_id_type fee_payer()const { return player; }
      void            validate()const;
      share_type      calculate_fee(const fee_parameters_type& )const;
    };
} } // graphene::chain

FC_REFLECT( graphene::chain::game_create_operation, (fee)(name)(description)(issuer)(script_code) )

FC_REFLECT( graphene::chain::game_update_operation, (fee)(issuer)(game_to_update)(new_description)(new_script_code)(new_issuer)(extensions) )

FC_REFLECT( graphene::chain::game_play_operation, (fee)(player)(game_to_play)(input_data) )

FC_REFLECT( graphene::chain::game_create_operation::fee_parameters_type, (basic_fee)(premium_fee)(price_per_kbyte) )
FC_REFLECT( graphene::chain::game_update_operation::fee_parameters_type, (fee)(price_per_kbyte) )
FC_REFLECT( graphene::chain::game_play_operation::fee_parameters_type, (fee)(price_per_kbyte) )
