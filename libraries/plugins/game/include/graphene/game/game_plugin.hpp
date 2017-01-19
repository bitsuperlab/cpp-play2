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

#include <graphene/app/plugin.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/game/v8_game.hpp>

#include <fc/thread/future.hpp>

namespace graphene { namespace game_plugin {

using namespace chain;

namespace detail
{
    class game_plugin_impl;
}

class game_plugin : public graphene::app::plugin {
public:
   game_plugin();
   virtual ~game_plugin();

   std::string plugin_name()const override;

   virtual void plugin_set_program_options(
      boost::program_options::options_description &command_line_options,
      boost::program_options::options_description &config_file_options
      ) override;

   virtual void plugin_initialize( const boost::program_options::variables_map& options ) override;
   virtual void plugin_startup() override;
   virtual void plugin_shutdown() override;

   void* get_isolate(/*const std::string& game_name*/);

   void execute( const signed_block& b, chain::database& db );

   v8_game_engine_ptr get_v8_engine(const std::string& game_name);

   bool reinstall_game_engine(const std::string& game_name);

   const game_object& get_game( const string& name );

   const asset_object& get_game_asset( const string& symbol );

   friend class detail::game_plugin_impl;
   std::unique_ptr<detail::game_plugin_impl> my;

private:

   boost::program_options::variables_map _options;
   bool _production_enabled = false;

   fc::future<void> _block_production_task;
};

} } //graphene::witness_plugin
