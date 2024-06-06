/*
	This file is part of solidity.

	solidity is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	solidity is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with solidity.  If not, see <http://www.gnu.org/licenses/>.
*/
// SPDX-License-Identifier: GPL-3.0
#include <libsolidity/interface/SMTSolverCommand.h>

#include <liblangutil/Exceptions.h>

#include <libsolutil/CommonIO.h>
#include <libsolutil/Exceptions.h>
#include <libsolutil/Keccak256.h>
#include <libsolutil/TemporaryDirectory.h>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/process.hpp>

namespace solidity::frontend
{

void SMTSolverCommand::setEldarica(std::optional<unsigned int> timeoutInMilliseconds, bool computeInvariants)
{
	m_arguments.clear();
	m_solverCmd = "eld";
	if (timeoutInMilliseconds)
	{
		unsigned int timeoutInSeconds = timeoutInMilliseconds.value() / 1000u;
		timeoutInSeconds = timeoutInSeconds == 0 ? 1 : timeoutInSeconds;
		m_arguments.push_back("-t:" + std::to_string(timeoutInSeconds));
	}
	if (computeInvariants)
		m_arguments.emplace_back("-ssol");
}

ReadCallback::Result SMTSolverCommand::solve(std::string const& _kind, std::string const& _query)
{
	try
	{
		if (_kind != ReadCallback::kindString(ReadCallback::Kind::SMTQuery))
			solAssert(false, "SMTQuery callback used as callback kind " + _kind);

		if (m_solverCmd.empty())
			return ReadCallback::Result{false, "No solver set."};

		auto tempDir = solidity::util::TemporaryDirectory("smt");
		util::h256 queryHash = util::keccak256(_query);
		auto queryFileName = tempDir.path() / ("query_" + queryHash.hex() + ".smt2");

		auto queryFile = boost::filesystem::ofstream(queryFileName);
		queryFile << _query << std::flush;

		auto eldBin = boost::process::search_path(m_solverCmd);

		if (eldBin.empty())
			return ReadCallback::Result{false, m_solverCmd + " binary not found."};

		auto args = m_arguments;
		args.push_back(queryFileName.string());

		boost::process::ipstream pipe;
		boost::process::child eld(
			eldBin,
			args,
			boost::process::std_out > pipe,
			boost::process::std_err >boost::process::null
		);

		std::vector<std::string> data;
		std::string line;
		while (eld.running() && std::getline(pipe, line))
			if (!line.empty())
				data.push_back(line);

		eld.wait();

		return ReadCallback::Result{true, boost::join(data, "\n")};
	}
	catch (...)
	{
		return ReadCallback::Result{false, "Unknown exception in SMTQuery callback: " + boost::current_exception_diagnostic_information()};
	}
}

}
