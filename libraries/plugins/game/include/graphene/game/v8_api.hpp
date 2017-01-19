#pragma once

#include <graphene/game/v8_helper.hpp>
#include <graphene/chain/database.hpp>

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

      static Persistent<FunctionTemplate> database_templ;

      static Persistent<FunctionTemplate> wallet_templ;
   };
    
    
    /**
     *  @class v8_database
     *  @brief wrappers database pointer to js object, database
     */
    class v8_database
    {
    public:
       v8_database(chain::database* db):_database(db){}

       static Local<Object> New(v8::Isolate* isolate, v8_database* local_v8_database);
       
       static void Modify(const v8::FunctionCallbackInfo<Value>& args);

       static void Create(const v8::FunctionCallbackInfo<Value>& args);

       static void Remove(const v8::FunctionCallbackInfo<Value>& args);

       //static void Get_Block(const v8::FunctionCallbackInfo<Value>& args);

       //static void Get_Transaction(const v8::FunctionCallbackInfo<Value>& args);

       //static void Get_Current_Random_Seed(const v8::FunctionCallbackInfo<Value>& args);

       //static void Get_Game_Data_Record(const v8::FunctionCallbackInfo<Value>& args);

       //static void Get_Account_Record_By_Name(const v8::FunctionCallbackInfo<Value>& args);

       // blockchain.get_asset_record(PLAY.game_asset.symbol);
       // static void Get_Asset_Record(const v8::FunctionCallbackInfo<Value>& args);

       //variables
       chain::database* _database;
       
       friend class chain::database;
       friend class db::object_database;  
    };
    

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
} } // bts::game
