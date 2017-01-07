#pragma once

#include <graphene/game/v8_helper.hpp>

#include <include/list.h>
#include <include/service.h>

#include <include/v8.h>
#include <include/libplatform/libplatform.h>

namespace graphene { namespace game_plugin {

   using namespace v8;

   class v8_api
   {
   public:
      /**
       * init the javascript classes
       */
      static bool init_class_template(v8::Isolate* isolate);

      /**
       * @brief Global method for create balance id for the owner of balance
       *
       */
      static void V8_Global_Get_Balance_ID_For_Owner(const v8::FunctionCallbackInfo<Value>& args);

      /**
       * @brief Method for getting transactions from full block
       *
       */
      static void V8_Block_Get_Transactions(const v8::FunctionCallbackInfo<Value>& args);

      static Persistent<ObjectTemplate> global;

      static Persistent<FunctionTemplate> blockchain_templ;

      static Persistent<FunctionTemplate> wallet_templ;

      static Persistent<FunctionTemplate> pendingstate_templ;

      static Persistent<FunctionTemplate> eval_state_templ;
   };
    
    
    /**
     *  @class v8_blockchain
     *  @brief wrappers blockchain pointer to js object, blockchain
     
    class v8_blockchain
    {
    public:
        v8_blockchain(chain_database_ptr blockchain, int block_num):_blockchain(blockchain), _block_num(block_num){}

        static Local<Object> New(v8::Isolate* isolate, v8_blockchain* local_v8_blockchain);

        static void Get_Block_Number(Local<String> property, const v8::PropertyCallbackInfo<v8::Value>& info);

        static void Get_Block_Digest(const v8::FunctionCallbackInfo<Value>& args);

        static void Get_Block(const v8::FunctionCallbackInfo<Value>& args);

        static void Get_Transaction(const v8::FunctionCallbackInfo<Value>& args);

        static void Get_Current_Random_Seed(const v8::FunctionCallbackInfo<Value>& args);

        static void Get_Game_Data_Record(const v8::FunctionCallbackInfo<Value>& args);

        static void Get_Account_Record_By_Name(const v8::FunctionCallbackInfo<Value>& args);

        // blockchain.get_asset_record(PLAY.game_asset.symbol);
        static void Get_Asset_Record(const v8::FunctionCallbackInfo<Value>& args);

        //variables
        chain_database_ptr _blockchain;
        uint32_t _block_num;
    };
    */

    /**
     *  @class v8_wallet
     *  @brief wrappers blockchain pointer to js object, wallet
     
    class v8_wallet
    {
    public:
        v8_wallet(wallet_ptr wallet):_wallet(wallet){}

        static Local<Object> New(v8::Isolate* isolate, v8_wallet* local_v8_wallet);

        static void Get_Transaction_Fee(const v8::FunctionCallbackInfo<Value>& args);

        static void Get_Wallet_Key_For_Address(const v8::FunctionCallbackInfo<Value>& args);

        static void Store_Transaction(const v8::FunctionCallbackInfo<Value>& args);

        //variables
        wallet_ptr _wallet;
    };
     */

    /**
     *  @class V8_PendingState
     *  @brief wrappers pendingstate pointer to js object
     
    class v8_chainstate
    {
    public:
        v8_chainstate(chain_interface_ptr chain_state): _chain_state(chain_state){}

        chain_interface_ptr _chain_state;

        static Local<Object> New(v8::Isolate* isolate, v8_chainstate* v8_pendingstate);

        static void Get_Blance_Record(const v8::FunctionCallbackInfo<Value>& args);

        static void Get_Asset_Record(const v8::FunctionCallbackInfo<Value>& args);

        static void Get_Game_Data_Record(const v8::FunctionCallbackInfo<Value>& args);

        static void Get_Account_Record_By_Name(const v8::FunctionCallbackInfo<Value>& args);

        static void Store_Blance_Record(const v8::FunctionCallbackInfo<Value>& args);

        static void Store_Asset_Record(const v8::FunctionCallbackInfo<Value>& args);

        static void Store_Game_Data_Record(const v8::FunctionCallbackInfo<Value>& args);
    };
     */

    /**
     *  @class V8_PendingState
     *  @brief wrappers pendingstate pointer to js object
     
    class v8_evalstate
    {
    public:
        v8_evalstate(transaction_evaluation_state* eval_state): _eval_state(eval_state){}

        transaction_evaluation_state* _eval_state;

        static Local<Object> New(v8::Isolate* isolate, v8_evalstate* local_v8_evalstate);

        static void Sub_Balance(const v8::FunctionCallbackInfo<Value>& args);

        static void Get_Transaction_Id(const v8::FunctionCallbackInfo<Value>& args);
    };
    */
} } // bts::game
