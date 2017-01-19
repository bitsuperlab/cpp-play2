#pragma once

#include <graphene/chain/exceptions.hpp>

#include <graphene/chain/protocol/game.hpp>
#include <graphene/chain/game_object.hpp>

namespace graphene { namespace game_plugin {
   using namespace std;
   using namespace graphene::chain;

   class game_plugin;

   namespace detail { class v8_game_engine_impl; }

    struct play_code
    {
        string from_account;
        string to_account;
        asset  amount;
        string memo;
    };

   /**
    * @class v8_game_engine
    *
    *  script context for javascript running
    */
   class v8_game_engine
   {
   public:
      v8_game_engine(std::string game_name, game_plugin& plugin);

      ~v8_game_engine(){};

      bool global( game_id_type game_id, asset_object game_asset);
      
      void evaluate( const game_play_operation& op, database& db );
      
      void apply( const game_play_operation& op, const game_play_object& obj, database& db );
       
      void execute( const signed_block& block, database& db );

   private:
      std::shared_ptr<detail::v8_game_engine_impl> my;
   };

   typedef std::shared_ptr<v8_game_engine> v8_game_engine_ptr;
} } // bts::graphene
