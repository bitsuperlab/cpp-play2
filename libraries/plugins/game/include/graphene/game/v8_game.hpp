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

      wallet_transaction_record play( game_id_type game_id, chain_database_ptr blockchain, bts::wallet::wallet_ptr w, const variant& var, bool sign );

      bool scan_ledger( chain_database_ptr blockchain, bts::wallet::wallet_ptr w,  wallet_transaction_record& trx_rec, const variant& var);

      bool scan_result( const game_result_transaction& rtrx,
                               uint32_t block_num,
                               const time_point_sec& block_time,
                               const uint32_t trx_index, bts::wallet::wallet_ptr w);
      */
      void execute( game_id_type game_id, uint32_t block_num);

   private:
      std::shared_ptr<detail::v8_game_engine_impl> my;
   };

   typedef std::shared_ptr<v8_game_engine> v8_game_engine_ptr;
} } // bts::graphene
