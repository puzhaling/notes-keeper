
#ifndef NOTES_CONTAINER_H
#define NOTES_CONTAINER_H

#include <initializer_list>
#include <string_view>
#include <cctype>
#include <string>
#include <vector>
#include <istream>
#include <limits>
#include <fstream>
#include <sstream>
#include <cassert>
#include <climits>
#include <algorithm>
#include <iostream>
#include <iomanip> // for std::quoted()
#include <stdio.h> // for remove()

#include "helpers.h"
#include "common.h"

using namespace std::literals;

class NotesContainer {
public:
	class BasicNote {
	public:
		enum Status : std::uint8_t {
			incompleted = 0,
			completed,
		};

	public:
		explicit BasicNote(std::string_view name = "") 
			: m_name{ name }, m_is_saved{ false }, m_status{ incompleted }
		{}

		static std::string status_to_str(Status status) {
			std::string res{};
			switch (status) {
				case incompleted: res = "incompleted"s; break;
				case completed: res = "completed"s; break;
			}
			return res;
		}

		friend std::istream& operator>>(std::istream& in, NotesContainer::BasicNote* note);

		auto get_name()   const { return m_name; }
		auto get_status() const { return m_status; }
		void change_status()    { m_status = (m_status == incompleted ? completed : incompleted); }
		void save() 			{ m_is_saved = true; }

		void set_name  (std::string_view name) { m_name = name; }
		void set_status(std::string_view status);

		virtual std::size_t size() const = 0;

	private:
		Status m_status;
		std::string m_name;
		bool m_is_saved;
	};

	class Document : public BasicNote {
	public:
		static constexpr std::uint16_t OUTPUT_WIDTH{ 100 };

	public:
		Document(std::string_view document_name = "")
			: BasicNote(document_name)
		{}

		static auto get_output_width() { return OUTPUT_WIDTH; }

		friend std::ostream& operator<<(std::ostream& out, NotesContainer::BasicNote* note);
		friend std::istream& operator>>(std::istream& in, NotesContainer::BasicNote* note);

		void 		set_text(const std::string& new_text) { m_text = new_text; }
		std::size_t size() const { return m_text.length(); } 

	private:
		std::string m_text;
	};	

	class TasksList : public BasicNote {
	public:
		struct Task {
			std::string text{}; 
			bool is_completed{ false };
		};

	public:
		TasksList() = default;
		TasksList(std::string_view name) : 
			BasicNote{ name } 
		{}
		TasksList(std::initializer_list<Task> tasks) {
			m_tasks.insert(std::end(m_tasks), std::begin(tasks), std::end(tasks));
		}

		friend std::ostream& operator<<(std::ostream& out, NotesContainer::BasicNote* note);
		friend std::istream& operator>>(std::istream& in, NotesContainer::BasicNote* note);

		auto& operator[](std::size_t idx)  { return m_tasks[idx]; }
		auto& add_task (const Task& task)  { m_tasks.emplace_back(task); return *this; }
		auto& get_tasks_container()        { return m_tasks; }


		std::size_t size() const { return m_tasks.size(); }

	private:
		std::vector<Task>m_tasks;
	};

public:

	std::vector<BasicNote*>::iterator begin() { return std::begin(m_notes); }
	std::vector<BasicNote*>::iterator end()   { return std::end(m_notes); }

	friend std::ostream& operator<<(std::ostream& out, NotesContainer::BasicNote* note);
	friend std::istream& operator>>(std::istream& in, NotesContainer::BasicNote* note);


	auto static get_note_type(BasicNote* note) -> std::string_view;
	auto 		add_note(std::istream& in) 	   -> NotesContainer&;

	auto* 		operator[](std::size_t idx) { return m_notes[idx]; }
	auto  		size() 	const 				{ return m_notes.size(); }

	void create_note();
	void delete_note(std::size_t idx);
	void print_notes_list() const;

private:
	std::vector<BasicNote*>m_notes;
};

#endif // NOTES_CONTAINER_H