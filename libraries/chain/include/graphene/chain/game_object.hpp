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
         static const uint8_t type_id  = impl_game_data_object_type;

         game_id_type                                     game_id;
         fc::variant                                      data;
   };

   class game_status_object : public abstract_object<game_status_object>
   {
      public:
         static const uint8_t space_id = implementation_ids;
         static const uint8_t type_id  = impl_game_status_object_type;

         game_id_type             game_id;
         uint32_t                 block_number;
         optional<fc::exception>  last_error;
   };

   class game_play_object : public abstract_object<game_play_object>
   {
      public:
         static const uint8_t space_id = implementation_ids;
         static const uint8_t type_id  = impl_game_play_object_type;

         game_id_type             game_id;
         fc::variant              input;
         uint16_t                 ref_block_num;
         transaction_id_type      ref_trx_id;
   };


   /**
    * @brief This class represents a game on the object graph
    * @ingroup object
    * @ingroup protocol
    *
    * Games are the primary unit of game script definition on the graphene system. Users must have an game before playing
    * with it, etc.
    */
   class game_object : public abstract_object<game_object>
   {
      public:
         static const uint8_t space_id = protocol_ids;
         static const uint8_t type_id  = game_object_type;

         /// The game's name. This name must be unique among all game names on the graph. May not be empty.
         string           name;
         string           description;
         account_id_type  issuer;
         string           script_code;

         game_id_type get_id()const { return id; }
   };


   struct by_game_name{};

   /**
    * @ingroup object_index
    */
   typedef multi_index_container<
      game_object,
      indexed_by<
         ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
         ordered_unique< tag<by_game_name>, member<game_object, string, &game_object::name> >
      >
   > game_multi_index_type;

   /**
    * @ingroup object_index
    */
   typedef generic_index<game_object, game_multi_index_type> game_index;

}}

FC_REFLECT_DERIVED( graphene::chain::game_data_object,
                    (graphene::db::object),
                    (game_id)(data)
                    )

FC_REFLECT_DERIVED( graphene::chain::game_status_object,
                    (graphene::db::object),
                    (game_id)(block_number)(last_error)
                    )

FC_REFLECT_DERIVED( graphene::chain::game_play_object,
                    (graphene::db::object),
                    (game_id)(input)(ref_block_num)(ref_trx_id)
                    )

FC_REFLECT_DERIVED( graphene::chain::game_object,
                    (graphene::db::object),
                    (name)(description)(issuer)
                    (script_code)
                    )
