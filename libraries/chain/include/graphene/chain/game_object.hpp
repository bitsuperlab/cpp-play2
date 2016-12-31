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
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/db/generic_index.hpp>
#include <boost/multi_index/composite_key.hpp>

namespace graphene { namespace chain {
   class database;

// https://github.com/dacsunlimited/dac_play/blob/master/libraries/blockchain/include/bts/blockchain/game_record.hpp
   /**
    * @brief Tracks the balance of a single account/asset pair
    * @ingroup object
    *
    * This object is indexed on owner and asset_type so that black swan
    * events in asset_type can be processed quickly.
    */
   class game_data_object : public abstract_object<game_data_object>
   {
      public:
         static const uint8_t space_id = implementation_ids;
         static const uint8_t type_id  = impl_game_record_object_type;

         game_id_type                                     game_id;
         fc::variant                                      data;

         asset get_balance()const { return asset(balance, asset_type); }
   };

   struct game_data_index
    {
        game_id_type        game_id;
        data_id_type        data_id;

        friend bool operator < ( const game_data_index& a, const game_data_index& b )
        {
            return std::tie( a.game_id, a.data_id ) < std::tie( b.game_id, b.data_id );
        }

        friend bool operator == ( const game_data_index& a, const game_data_index& b )
        {
            return std::tie( a.game_id, a.data_id ) == std::tie( b.game_id, b.data_id );
        }
    };


    class game_status_object : public abstract_object<game_status_object>
    {
       public:
          static const uint8_t space_id = implementation_ids;
          static const uint8_t type_id  = impl_game_record_object_type;

          game_id_type             game_id;
          uint32_t                 block_number;
          optional<fc::exception>  last_error;

          asset get_balance()const { return asset(balance, asset_type); }
    };


   /**
    * @brief This class represents an account on the object graph
    * @ingroup object
    * @ingroup protocol
    *
    * Accounts are the primary unit of authority on the graphene system. Users must have an account in order to use
    * assets, trade in the markets, vote for committee_members, etc.
    */
   class game_object : public graphene::db::abstract_object<game_object>
   {
      public:
         static const uint8_t space_id = protocol_ids;
         static const uint8_t type_id  = game_object_type;

         enum
         {
             god_owner_id     =  0,
             null_owner_id    = -1
         };

         game_record make_null()const;
         bool is_null()const               { return owner_account_id == null_owner_id; };

         game_id_type        id;
         std::string         name;
         std::string         description;
         fc::variant         public_data;
         account_id_type     owner_account_id;
         std::string         script_code;
         fc::time_point_sec  registration_date;
         fc::time_point_sec  last_update;

         /** reserved for future extensions */
         vector<char>        reserved;

         account_id_type get_id()const { return id; }
   };

}}

FC_REFLECT_DERIVED( graphene::chain::account_object,
                    (graphene::db::object),
                    (membership_expiration_date)(registrar)(referrer)(lifetime_referrer)
                    (network_fee_percentage)(lifetime_referrer_fee_percentage)(referrer_rewards_percentage)
                    (name)(owner)(active)(options)(statistics)(whitelisting_accounts)(blacklisting_accounts)
                    (whitelisted_accounts)(blacklisted_accounts)
                    (cashback_vb)
                    (owner_special_authority)(active_special_authority)
                    (top_n_control_flags)
                    (allowed_assets)
                    )

FC_REFLECT_DERIVED( graphene::chain::account_balance_object,
                    (graphene::db::object),
                    (owner)(asset_type)(balance) )

FC_REFLECT_DERIVED( graphene::chain::account_statistics_object,
                    (graphene::chain::object),
                    (owner)
                    (most_recent_op)
                    (total_ops)
                    (total_core_in_orders)
                    (lifetime_fees_paid)
                    (pending_fees)(pending_vested_fees)
                  )
