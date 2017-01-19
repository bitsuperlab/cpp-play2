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
      /*
      void evaluate( transaction_evaluation_state& eval_state, game_id_type game_id, const variant& var);
      */
      void execute( game_id_type game_id, uint32_t block_num);

   private:
      std::shared_ptr<detail::v8_game_engine_impl> my;
   };

   typedef std::shared_ptr<v8_game_engine> v8_game_engine_ptr;
} } // bts::graphene
