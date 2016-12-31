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
#include <graphene/chain/protocol/buyback.hpp>
#include <graphene/chain/protocol/ext.hpp>
#include <graphene/chain/protocol/special_authority.hpp>
#include <graphene/chain/protocol/types.hpp>
#include <graphene/chain/protocol/vote.hpp>

namespace graphene { namespace chain {

   bool is_valid_name( const string& s );
   bool is_cheap_name( const string& n );

   /// These are the fields which can be updated by the active authority.
   struct account_options
   {
      /// The memo key is the key this account will typically use to encrypt/sign transaction memos and other non-
      /// validated account activities. This field is here to prevent confusion if the active authority has zero or
      /// multiple keys in it.
      public_key_type  memo_key;
      /// If this field is set to an account ID other than GRAPHENE_PROXY_TO_SELF_ACCOUNT,
      /// then this account's votes will be ignored; its stake
      /// will be counted as voting for the referenced account's selected votes instead.
      account_id_type voting_account = GRAPHENE_PROXY_TO_SELF_ACCOUNT;

      /// The number of active witnesses this account votes the blockchain should appoint
      /// Must not exceed the actual number of witnesses voted for in @ref votes
      uint16_t num_witness = 0;
      /// The number of active committee members this account votes the blockchain should appoint
      /// Must not exceed the actual number of committee members voted for in @ref votes
      uint16_t num_committee = 0;
      /// This is the list of vote IDs this account votes for. The weight of these votes is determined by this
      /// account's balance of core asset.
      flat_set<vote_id_type> votes;
      extensions_type        extensions;

      void validate()const;
   };

   /**
    *  @ingroup operations
    */
    struct create_game_operation : public base_operation
    {
      ///Names are a more complete description and may contain any kind of characters or spaces.
      std::string      name;

      /// Describes the asset and its purpose.
      std::string      description;

      /// Other information relevant to this asset.
      fc::variant      public_data;

      /// Game only be issued by individuals that have registered a name.
      account_id_type  owner_account_id;

      /// The url of the game's rule script
      std::string      script_code;

      account_id_type fee_payer()const { return registrar; }
      void            validate()const;
      share_type      calculate_fee(const fee_parameters_type& )const;

      void get_required_active_authorities( flat_set<account_id_type>& a )const
      {
         // registrar should be required anyway as it is the fee_payer(), but we insert it here just to be sure
         a.insert( registrar );
         if( extensions.value.buyback_options.valid() )
            a.insert( extensions.value.buyback_options->asset_to_buy_issuer );
      }
    };

    struct game_update_operation : public base_operation
    {

      game_id_type               game_id;

      /// Describes the asset and its purpose.
      std::string      description;

      /// Other information relevant to this asset.
      fc::variant      public_data;

      /// The url of the game's rule script
      std::string      script_code;

      account_id_type fee_payer()const { return registrar; }
      void            validate()const;
      share_type      calculate_fee(const fee_parameters_type& )const;

      void get_required_active_authorities( flat_set<account_id_type>& a )const
      {
         // registrar should be required anyway as it is the fee_payer(), but we insert it here just to be sure
         a.insert( registrar );
         if( extensions.value.buyback_options.valid() )
            a.insert( extensions.value.buyback_options->asset_to_buy_issuer );
      }
    };

    struct game_play_operation : public base_operation
    {

      bts::game::game_input  input;

      account_id_type fee_payer()const { return registrar; }
      void            validate()const;
      share_type      calculate_fee(const fee_parameters_type& )const;

      void get_required_active_authorities( flat_set<account_id_type>& a )const
      {
         // registrar should be required anyway as it is the fee_payer(), but we insert it here just to be sure
         a.insert( registrar );
         if( extensions.value.buyback_options.valid() )
            a.insert( extensions.value.buyback_options->asset_to_buy_issuer );
      }
    };
} } // graphene::chain

FC_REFLECT( graphene::chain::create_game_operation, (name)(description)(public_data)(owner_account_id)(script_code) )

FC_REFLECT( graphene::chain::game_update_operation, (game_id)(description)(public_data)(script_code) )

FC_REFLECT( graphene::chain::game_play_operation, (input) )
